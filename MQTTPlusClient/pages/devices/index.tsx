"use client";

import React, { useEffect, useState } from "react";
import { MQTTPlusProvider } from "../../app/client/mqttplus";
import Table from "@/Tables/Table";
import Breadcrumb from "@/Breadcrumb";
import { ReadyState } from "react-use-websocket";
import toast from "react-hot-toast";
import { useRouter } from "next/navigation";
import { NextPage } from "next";
import { GetDeviceName, GetId } from "@/Utility";
import { ConnectionStatus } from "@/DeviceComponents";

interface Props {}

const DevicePage: NextPage<Props> = props => {

  const provider = MQTTPlusProvider()
  const api = provider.api

  const router = useRouter()

  const [devices, setDevices] = useState<any | null>(null)
  const [event, setEvent] = useState<any | null>()

  useEffect(() => {
    if (provider.status != ReadyState.OPEN) return;

    provider.receive((data, error) => {
      ProcessResponse(data)
    })

    provider.post(api.devices({
      expands: ["data.network"]
    }))
    provider.post(api.event({
      listen: ['mqtt.client_connection_change']
    }))

  }, [provider.status])

  useEffect(() => {
    if (!event) return

    switch (event.event) {
      case "mqtt.client_connection_change":
        HandleConnectionStatusChange(event.data)
        break
    }
    setEvent(null)
    return
  }, [event])


  function HandleConnectionStatusChange(data: any) {
    if (data.status == 1)
      toast.success(`MQTT ${data.client_id} connected`)
    else toast.error(`MQTT ${data.client_id} disconnected`)

    const dev = devices
    const index = devices.findIndex(device => device.client_id == data.client_id)

    if (index != -1) {
      dev[index].status = data.status
      dev[index].last_seen = data.last_seen
    }
    else {
      dev.push(data)
    }

    setDevices(dev)
  }

  function OpenDevicePage(device: any) {
    router?.replace(`/devices/${device.id}`)
  }

  return (
    <>
      <Breadcrumb pageName="Devices" />
      <Table label="Services" columns={["Device", "Network", "Status", "Last seen"]} >
        {devices?.map(device => {
          return <DeviceRow device={device} />
        })}
        {JSON.stringify(devices)}
      </Table>
    </>
  )

  function DeviceRow(props: { device: any })
  {
    const { device } = props
    return (
      <div className="grid grid-cols-4 border-b border-stroke dark:border-strokedark hover:bg-meta-4 hover:bg-opacity-50"
        key={device.id}
        onClick={() => { OpenDevicePage(device) }}
      >
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {GetDeviceName(device)}
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {GetId(device.network)}
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          <ConnectionStatus status={device.status} />
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {new Date(device.last_seen).toLocaleString()}
        </div>
      </div>
    )
  }

  function ProcessResponse(data: any) {
    if (data == null) return

    if (data.event) {
      setEvent(data)
      return
    }

    switch (data.type) {
      case 'devices':
        setDevices(data.data)
        break
    }
  }
}

export default DevicePage