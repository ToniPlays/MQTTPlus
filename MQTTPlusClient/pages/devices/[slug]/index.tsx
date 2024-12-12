import React, { useEffect, useState } from "react"
import { NextPage } from "next"
import { useRouter } from 'next/router'
import { MQTTPlusProvider } from "../../../app/client/mqttplus"
import Breadcrumb from "@/Breadcrumb"
import Table from "@/Tables/Table"
import { Grid } from "@mui/material"
import { ConnectionStatus } from "@/DeviceComponents"
import { GetDeviceName } from "@/Utility"

interface Props
{

}

// /devices/9dd2852972f9a0e7
// /device?id=9dd2852972f9a0e7

const DevicePage: NextPage<Props> = props => {
  const router = useRouter()
  const provider = MQTTPlusProvider()
  const api = provider.api

  const [deviceId, setDeviceId] = useState<string | null>(null)
  const [deviceData, setDeviceData] = useState<any | null>(null)

  useEffect(() => {
    if(!router.query.slug) return

    setDeviceId(router.query.slug as string)
  }, [router])

  useEffect(() => {
    provider.post(api.device(deviceId!, {
      expands: []
    }))

    provider.receive((data, error) => {
      HandleResponse(data)
    })
  }, [deviceId])

  if(!deviceData)
    return (<></>)

  return (
    <>
      <Breadcrumb pageName={GetDeviceName(deviceData)} />
      <div className="sticky top-0 z-999 flex w-full bg-white drop-shadow-1 dark:bg-boxdark dark:drop-shadow-none">
        <DeviceHeader device={deviceData} />
      </div>
    </>
  )

  function DeviceHeader(props: { device: any})
  {
    const { device } = props
    return (
      <div className="m-4 grid gap-4 md:grid-cols-2 md:gap-6 xl:grid-cols-2 2xl:gap-7.5" style={{ width: "100%"}}>
        <p>Client: {device.client_id}</p>
        <div className="text-right">
          <ConnectionStatus status={device.status} />
        </div>
      </div>
    )
  } 

  function HandleResponse(data: any)
  {
    switch(data.type)
    {
      case "device":
        setDeviceData(data.data)
        break
    }
  }
}

export default DevicePage