"use client";

import React, { useEffect, useState } from "react";
import DefaultLayout from "@/DefaultLayout";
import CardDataStats from "@/CardDataStats";
import { MQTTPlusProvider } from "../../app/client/mqttplus"
import {
  CheckCircleIcon,
  CpuChipIcon,
  EyeIcon,
  ServerIcon,
  TrashIcon,
} from "@heroicons/react/24/solid";
import { ReadyState } from "react-use-websocket";
import { useTimeAgo } from "react-time-ago";
import en from "javascript-time-ago/locale/en";
import TimeAgo from "javascript-time-ago";

import {
  Capitalize,
  FormatBytes,
  FormatPercentage,
  GetDeviceName,
} from "../../components/Utility";
import Breadcrumb from "@/Breadcrumb";
import Table from "@/Tables/Table";
import toast from "react-hot-toast";
import { NextPage } from "next";

TimeAgo.addDefaultLocale(en);

interface Props
{
}

const ServerPage: NextPage<Props> = props => {

  const provider = MQTTPlusProvider();
  const api = provider.api;
  const [serverStatus, setServerStatus] = useState<any | null>(null);
  const [systemUsage, setSystemUsage] = useState<any | null>(null);

  const since = useTimeAgo({
    date: new Date(serverStatus?.startup_time ?? 0),
    locale: "en",
  });

  useEffect(() => {
    if (provider.status != ReadyState.OPEN) {
      setServerStatus(null)
      return;
    }

    provider.receive((message, error) => {
      ProcessMessage(message)
    })

    provider.post(
      api.server({
        expands: [
          'data.status', 'data.services'
        ],
      }),
    );

    provider.post(
      api.event({
        listen: ['server.status', 'mqtt.client_connection_change']
      })
    )
  }, [provider.status]);

  function GetServiceText() {
    return `${serverStatus?.running_service_count ?? 0} / ${serverStatus?.service_count ?? 0}`;
  }

  const memoryUsed = systemUsage?.memory_total - systemUsage?.memory_available;

  return (
    <>
      <Breadcrumb pageName="Server" />
      <div className="grid grid-cols-1 gap-4 md:grid-cols-2 md:gap-6 xl:grid-cols-3 2xl:gap-7.5">
        <CardDataStats title="Status" total={GetStatus()}>
          <CheckCircleIcon
            width={36}
            height={36}
            color={provider.status == ReadyState.OPEN ? "green" : "red"}
          />
        </CardDataStats>
        <CardDataStats title="Services" total={GetServiceText()}>
          <EyeIcon width={36} height={36} />
        </CardDataStats>
        <CardDataStats
          title={"Running"}
          total={serverStatus ? Capitalize(since.formattedDate) : "---"}
          rate={serverStatus ? since.date.toLocaleString() : ""}
        >
          <TrashIcon width={36} height={36} />
        </CardDataStats>
        <CardDataStats
          title="CPU"
          total={`${FormatPercentage(systemUsage?.cpu_load ?? 0)}%`}
        >
          <CpuChipIcon width={36} height={36} />
        </CardDataStats>

        <CardDataStats
          title="Memory"
          total={`${FormatPercentage(memoryUsed / systemUsage?.memory_total)}%`}
          rate={`${FormatBytes(memoryUsed)} / ${FormatBytes(systemUsage?.memory_total)}`}
        >
          <ServerIcon width={36} height={36} />
        </CardDataStats>

        <CardDataStats
          title="Disk"
          total={`${FormatBytes(systemUsage?.disk_space_used)} / ${FormatBytes(systemUsage?.disk_space_total)}`}
          rate={`${((systemUsage?.disk_space_used / systemUsage?.disk_space_total) * 100).toFixed(2)}%`}
        >
          <ServerIcon width={36} height={36} />
        </CardDataStats>
      </div>
      <br />
        <br />
      <br /> 
      <div className="gap-4 md:gap-6 2xl:gap-7.5">
        <Table label="Services" columns={["Name", "Started at", "Status"]} >
          {serverStatus?.services.map(service => {
            return (
              <div className="grid grid-cols-3 border-b border-stroke dark:border-strokedark"
                key={service.name}
              >
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    {service.name}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                  {new Date(service.startup_time).toLocaleString()}
                </div>
                <div className="flex items-center gap-3 p-2.5 xl:p-5">
                    <p className={service.running ? "text-meta-3" : "text-meta-1"}>{service.running ? "Running" : "Stopped"}</p>
                </div>
              </div>
            )
          })}
        </Table>
      </div>
      </>
  )
  
  function GetStatus() {
    switch (provider.status) {
      case ReadyState.CLOSED:
        return "No connection";
      case ReadyState.CONNECTING:
        return "Connecting";
      case ReadyState.OPEN:
        return "Connected";
    }
    return "Unknown";
  }

  function ProcessMessage(data: any)
  {
    if(data == null) return

    if(data.event)
    {
      HandleEvent(data)
      return
    }

    switch(data.type)
    {
      case 'server':
        setServerStatus(data.data)
        break
    }
  }

  function HandleEvent(event: any)
  {
    switch(event.event)
    {
      case 'server.status':
      {
        setSystemUsage(event.data)
        break
      }
      case 'mqtt.client_connection_change':
      {
        if(event.data.status == 1)
          toast.success(`MQTT ${GetDeviceName(event.data)} connected`)
        else toast.error(`MQTT ${GetDeviceName(event.data)} disconnected`)
        break
      }
    }
  }
}

export default ServerPage