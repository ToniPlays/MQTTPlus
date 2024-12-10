"use client";

import React, { useEffect, useRef, useState } from "react";
import DefaultLayout from "../../components/DefaultLayout";
import { MQTTPlusProvider } from "../../client/mqttplus";
import Table from "../../components/Tables/Table";
import Breadcrumb from "../../components/Breadcrumb";
import { ReadyState } from "react-use-websocket";
import toast from "react-hot-toast";


export default function Devices() {
  const provider = MQTTPlusProvider()
  const api = provider.api

  const [devices, setDevices] = useState<any | null>(null)
  const [event, setEvent] = useState<any | null>()
  const ref = useRef(devices)

  useEffect(() => 
  {
    if(provider.status != ReadyState.OPEN) return;
    
    provider.receive((data, error) => {
      ProcessResponse(data)      
    })

    provider.post(api.devices({
      expands: ['data.devices']
    }))
    provider.post(api.event({
      listen: ['mqtt.client_connection_change']
    }))

  }, [provider.status])

  useEffect(() => {
    if(!event) return
    
    switch(event.event)
    {
      case "mqtt.client_connection_change":
        HandleConnectionStatusChange(event.data)
        break
    }
    setEvent(null)
    return
  }, [event])

  
  function HandleConnectionStatusChange(data: any)
  {
    console.log(data)
    if(data.status == 1)
      toast.success(`MQTT ${data.client_id} connected`)
    else toast.error(`MQTT ${data.client_id} disconnected`)

    const dev = devices
    const index = devices.findIndex(device => device.client_id == data.client_id)

    if(index != -1)
    {
      dev[index].status = data.status
      dev[index].last_seen = data.last_seen      
    }
    else 
    {
      dev.push(data)
    }
    
    console.log(dev)
    setDevices(dev)
  }

  return (
    <DefaultLayout>
      <Breadcrumb pageName="Devices" />
      <Table label="Services" columns={["Device", "Network", "Status", "Last seen"]} >
          {devices?.map(device => {
            return (
              <div className="grid grid-cols-4 border-b border-stroke dark:border-strokedark"
                  key={device.client_id}
              >
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    {device.nickname?.length > 0 ? device.nickname : device.client_id}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                  {device.network}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    <p className={device.status ? "text-meta-3" : "text-meta-1"}>{device.status ? "Connected" : "Disconnected"}</p>
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                  {new Date(device.last_seen).toLocaleString()}
                </div>
              </div>
            )
          })}
          {JSON.stringify(devices)}
        </Table>
    </DefaultLayout>
  )

  function ProcessResponse(data: any)
  {
    if(data == null) return

    if(data.event)
    {
      setEvent(data)
      return
    }

    switch(data.type)
    {
      case 'devices':
        setDevices(data.data)
        break
    }
  }
}
