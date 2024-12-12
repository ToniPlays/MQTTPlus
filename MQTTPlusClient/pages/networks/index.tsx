"use client";

import React, { useEffect, useRef, useState } from "react";
import DefaultLayout from "@/DefaultLayout";
import { MQTTPlusProvider } from "../../app/client/mqttplus";
import Table from "@/Tables/Table";
import Breadcrumb from "@/Breadcrumb";
import { ReadyState } from "react-use-websocket";
import toast from "react-hot-toast";
import { useRouter } from "next/navigation";
import { NextPage } from "next";
import { GetDeviceName } from "@/Utility";
import { ConnectionStatus } from "@/DeviceComponents";

interface Props {

}

const NetworkPage: NextPage<Props> = props => {

  const provider = MQTTPlusProvider()
  const api = provider.api

  const router = useRouter()

  const [networks, setNetworks] = useState<any | null>(null)
  const [event, setEvent] = useState<any | null>()

  useEffect(() => {
    if (provider.status != ReadyState.OPEN) return;

    provider.receive((data, error) => {
      ProcessResponse(data)
    })

    provider.post(api.networks({
      expands: []
    }))

  }, [provider.status])

  useEffect(() => {
    if (!event) return

    setEvent(null)
  }, [event])


  function OpenDevicePage(device: any) {
    router?.replace(`/devices/${device.id}`)
  }

  return (
    <>
      <Breadcrumb pageName="Networks" />
      <Table label="Services" columns={["Network", "Type", "Active devices", "Total devices"]} >
        {networks?.map(network => {
          return <NetworkRow network={network} />
        })}
      </Table>
    </>
  )

  function NetworkRow(props: { device: any })
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
          {device.network}
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
      case 'networks':
        setNetworks(data.data)
        break
    }
  }
}

export default NetworkPage