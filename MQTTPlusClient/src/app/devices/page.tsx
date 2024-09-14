"use client";

import React, { useEffect, useState } from "react";
import DefaultLayout from "../../components/DefaultLayout";
import { MQTTPlusProvider } from "../../client/mqttplus";
import Table from "../../components/Tables/Table";
import Breadcrumb from "../../components/Breadcrumb";
import { ReadyState } from "react-use-websocket";
import { MQTTPlus as Events } from "../../client/types/Events";
import toast from "react-hot-toast";


interface Device {
  uid: string
  device_name: string
  status: string
  last_seen: string
}

export default function Devices() {
  const provider = MQTTPlusProvider()
  const api = provider.api

  const [devices, setDevices] = useState<Device[]>([])

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
      
      const type = data.data.type
      const eventData = data.data.event_data
      
      switch(type)
      {
        case Events.Events.EventType.MQTTClientConnectionStatusChanged:
          if(eventData.is_connected)
            toast.success(`MQTT ${eventData.client_id} connected`)
          else toast.error(`MQTT ${eventData.client_id} disconnected`)
          break 
      }
    })

  }, [provider.status])

  function GetStatusColor(status: string)
  {
    switch(status)
    {
        case 'Disconnected': return 'text-meta-1'
        case 'Connected': return 'text-meta-3'
    }
  }

  return (
    <DefaultLayout>
      <Breadcrumb pageName="Devices" />
      <Table label="Services" columns={["Device", "Network", "Status", "Last seen"]} >
          {devices?.map(device => {
            return (
              <div className="grid grid-cols-4 border-b border-stroke dark:border-strokedark">
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    {device.device_name}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                  None
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
