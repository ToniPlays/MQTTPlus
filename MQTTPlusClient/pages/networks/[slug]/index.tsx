import React, { useEffect, useState } from "react"
import { NextPage } from "next"
import { useRouter } from 'next/router'
import { MQTTPlusProvider } from "../../../app/client/mqttplus"
import Breadcrumb from "@/Breadcrumb"

interface Props
{

}

const DevicePage: NextPage<Props> = props => {
  const router = useRouter()
  const provider = MQTTPlusProvider()
  const api = provider.api

  const [networkId, setNetworkId] = useState<string | null>(null)
  const [networkData, setNetworkData] = useState<any | null>(null)

  useEffect(() => {
    if(!router.query.slug) return

    setNetworkId(router.query.slug as string)
  }, [router])

  useEffect(() => {
    provider.post(api.network(networkId!, {
      expands: []
    }))

    provider.receive((data, error) => {
      HandleResponse(data)
    })
  }, [networkId])

  if(!networkId)
    return (<></>)

  return (
    <>
      <Breadcrumb pageName={networkData?.name ?? ""} />
      <div className="sticky top-0 z-999 flex w-full bg-white drop-shadow-1 dark:bg-boxdark dark:drop-shadow-none">
        {networkId ? <NetworkHeader network={networkData} /> : null }
      </div>
    </>
  )

  function NetworkHeader(props: { network: any})
  {
    const { network } = props
    return (
      <div className="m-4 grid gap-4 md:grid-cols-2 md:gap-6 xl:grid-cols-2 2xl:gap-7.5" style={{ width: "100%"}}>
        <div className="text-right">
        </div>
      </div>
    )
  } 

  function HandleResponse(data: any)
  {
    switch(data.type)
    {
      case "network":
        setNetworkData(data.data)
        break
    }
  }
}

export default DevicePage