"use client";

import React, { useEffect, useState } from "react";
import DefaultLayout from "../../components/DefaultLayout";
import CardDataStats from "../../components/CardDataStats";
import { MQTTPlusProvider } from "../../client/mqttplus";
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
import { MQTTPlus as Serv } from "../../client/types/Server";
import { MQTTPlus as Events } from "../../client/types/Events";
import {
  Capitalize,
  FormatBytes,
  FormatPercentage,
} from "../../components/Utility";
import Breadcrumb from "../../components/Breadcrumb";

TimeAgo.addDefaultLocale(en);

export default function Server() {
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
      setServerStatus(null);
      return;
    }

    provider.post(
      api.server.status({
        expands: [
          Serv.Server.ExpandOpts.Services,
          Serv.Server.ExpandOpts.Status,
        ],
      }),
    );

    provider.post(
      api.events.listen({
        listen: [
          {
            type: Events.Events.EventType.ServerStatus,
            interval: 5.0,
          },
        ],
      }),
    );
  }, [provider.status]);

  function SetEndpoints() {
    provider.receive(api.server.endpoint, (data, error) => {
      if (!error) {
        setServerStatus(data.data);
        setSystemUsage(data.data.status);
        since.date = new Date(serverStatus?.startup_time);
      }
    });

    provider.receive(api.events.endpoint, (data, error) => {
      const type = data["data"]["type"];
      if (type == Events.Events.EventType.ServerStatus)
        setSystemUsage(data["data"]["event_data"]);
    });
  }

  SetEndpoints();

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

  function GetServiceText() {
    return `${serverStatus?.running_service_count ?? 0} / ${serverStatus?.service_count ?? 0}`;
  }

  const memoryUsed = systemUsage?.memory_total - systemUsage?.memory_available;

  return (
    <DefaultLayout>
      <Breadcrumb pageName="Server" />
      <div className="grid grid-cols-1 gap-4 md:grid-cols-2 md:gap-6 xl:grid-cols-3 2xl:gap-7.5">
        <CardDataStats title={"Status"} total={GetStatus()}>
          <CheckCircleIcon
            width={36}
            height={36}
            color={provider.status == ReadyState.OPEN ? "green" : "red"}
          />
        </CardDataStats>
        <CardDataStats title={"Services"} total={GetServiceText()}>
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
          title={"CPU"}
          total={`${FormatPercentage(systemUsage?.cpu_load ?? 0)}%`}
        >
          <CpuChipIcon width={36} height={36} />
        </CardDataStats>

        <CardDataStats
          title={"Memory"}
          total={`${FormatPercentage(memoryUsed / systemUsage?.memory_total)}%`}
          rate={`${FormatBytes(memoryUsed)} / ${FormatBytes(systemUsage?.memory_total)}`}
        >
          <ServerIcon width={36} height={36} />
        </CardDataStats>

        <CardDataStats
          title={"Disk"}
          total={`${FormatBytes(systemUsage?.disk_space_used)} / ${FormatBytes(systemUsage?.disk_space_total)}`}
          rate={`${((systemUsage?.disk_space_used / systemUsage?.disk_space_total) * 100).toFixed(2)}%`}
        >
          <ServerIcon width={36} height={36} />
        </CardDataStats>
      </div>
    </DefaultLayout>
  );
}
