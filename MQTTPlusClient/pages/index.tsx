import React, { useState } from 'react'
import { Button } from '@mui/material'

import { NextPage } from 'next'
import { MQTTPlusProvider } from '../client/mqttplus'
import { MQTTPlus } from '../client/types/Server'

interface Props {

};

const Home: NextPage<Props> = props => {

  const provider = MQTTPlusProvider()
  const api = provider.api
  const [message, setMessage] = useState("")

  function TestPost(message: string)
  {
    
    provider.post(api.server.status({ expands: [MQTTPlus.Server.ExpandOpts.Services] }));
  }

  provider.receive(api.server.endpoint, (response, error) => {
    setMessage(JSON.stringify(response))
  })
  
  return (
      <>
        <p>Socket: {provider.status}</p>
        <p>Latest message: {message}</p>
        <Button className='mt-8' variant='contained' onClick={() => TestPost("Random stuff") }>Button</Button>
      </>
  )
}

export default Home
