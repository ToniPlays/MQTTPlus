"use client";

import React, { useEffect, useState } from "react"
import { MQTTPlusProvider } from "../../app/client/mqttplus"
import Table from "@/Tables/Table"
import Breadcrumb from "@/Breadcrumb"
import { ReadyState } from "react-use-websocket"
import { useRouter } from "next/navigation"
import { NextPage } from "next"
import { ConnectionStatus } from "@/DeviceComponents"

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

    provider.post(api.networks())

  }, [provider.status])

  useEffect(() => {
    if (!event) return

    setEvent(null)
  }, [event])


  function OpenNetworkPage(network: any) {
    router?.replace(`/networks/${network.id}`)
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
    const { network } = props

    return (
      <div className="grid grid-cols-4 border-b border-stroke dark:border-strokedark hover:bg-meta-4 hover:bg-opacity-50"
        key={network.id}
        onClick={() => { OpenNetworkPage(network) }}
      >
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {network.name}
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {network.type}
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
         {network.active_devices}
        </div>
        <div className="flex items-center gap-3 p-2.5 xl:p-5">
          {network.total_devices}
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