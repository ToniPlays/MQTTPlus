"use client";

import React from "react";
import DefaultLayout from "../../components/DefaultLayout";
import { MQTTPlusProvider } from "../../client/mqttplus";
import Table from "../../components/Tables/Table";
import Breadcrumb from "../../components/Breadcrumb";


interface Device {
  uid: string
  name: string
  status: string
}

export default function Server() {
  const provider = MQTTPlusProvider()
  const api = provider.api

  const data: Device[] = [
    {
      uid: "2312365654",
      name: "Test device",
      status: "Connected",
    },
    {
      uid: "32547440",
      name: "Test device 2",
      status: "Disconnected",
    },
  ];

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
      <Table label="My devices" columns={["Device", "Status", "Last seen"]}>
      {data.map(device => {
        return (
            <div
            className={`flex justify-between`}
            key={device.uid}
          >
            <div className="flex items-center gap-3">
              <div className="flex-shrink-0">
                
              </div>
              <p className="hidden text-black dark:text-white sm:block">
                {device.name}
              </p>
            </div>

            <div className="flex items-center justify-center p-2.5 xl:p-5">
              <p className={GetStatusColor(device.status)}>{device.status}</p>
            </div>

            <div className="flex items-center justify-center p-2.5 xl:p-5">
              <p className="text-meta-0">5 minutes ago</p>
            </div>
          </div>)
      })}
      </Table>
    </DefaultLayout>
  );
}
