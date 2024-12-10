import React, { useEffect, useState } from "react"
import { NextPage } from "next"
import { useRouter } from 'next/router'
import { MQTTPlusProvider } from "../../../app/client/mqttplus"

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

  useEffect(() => {
    if(!router.query.slug) return

    setDeviceId(router.query.slug as string)
  }, [router])

  useEffect(() => {
    provider.post(api.device(deviceId!, {
      expands: []
    }))
  }, [deviceId])


  return (
    <p>{deviceId}</p>
  )
}

export default DevicePage