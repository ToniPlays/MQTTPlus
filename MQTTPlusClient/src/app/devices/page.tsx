"use client";

import React, { useEffect, useRef, useState } from "react";
import DefaultLayout from "../../components/DefaultLayout";
import { MQTTPlusProvider } from "../../client/mqttplus";
import Table from "../../components/Tables/Table";
import Breadcrumb from "../../components/Breadcrumb";
import { ReadyState } from "react-use-websocket";
import { MQTTPlus as Events } from "../../client/types/Events";
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
    
    provider.post(api.devices.list())
    provider.receive(api.devices.endpoint, (data, error) => {
      setDevices(data.data)
    })

    provider.post(api.events.listen({
      listen: [{
        type: Events.Events.EventType.MQTTClientConnectionStatusChanged,
      }]
    }))

    provider.receive(api.events.endpoint, (data, error) => {
      setEvent(data.data)
    })

  }, [provider.status])

  useEffect(() => {
    if(event == null) return

    const type = event.type
    const eventData = event.event_data
    switch(type)
      {
        case Events.Events.EventType.MQTTClientConnectionStatusChanged:
          HandleConnectionStatusChange(eventData)
          break 
      }
    setEvent(null)
  },[event])
  
  function HandleConnectionStatusChange(data: any)
  {
    if(data.status == 1)
      toast.success(`MQTT ${data.device_name} connected`)
    else toast.error(`MQTT ${data.device_name} disconnected`)

    const dev = devices;
    const index = devices.findIndex(device => device.id == data.id)

    if(index != -1)
    {
      dev[index].status = data.status
      dev[index].last_seen = data.last_seen
      setDevices(dev)
    }
  }

  return (
    <DefaultLayout>
      <Breadcrumb pageName="Devices" />
      <Table label="Services" columns={["Device", "Network", "Status", "Last seen"]} >
          {devices?.map(device => {
            return (
              <div className="grid grid-cols-4 border-b border-stroke dark:border-strokedark"
                  key={device.id}
              >
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    {device.device_name}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                  SomeNetwork
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
        </Table>
    </DefaultLayout>
  );
}
