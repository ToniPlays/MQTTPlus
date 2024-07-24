import React, { useState } from 'react'
import { Button } from '@mui/material'

import { NextPage } from 'next'
import { mqttPlusProvider } from '../client/mqttplus'

interface Props {

};

const Home: NextPage<Props> = props => {

  const provider = mqttPlusProvider()
  const [message, setMessage] = useState("")

  function TestPost(message: string)
  {
    provider.post({ 
      endpoint: '/mqtt',
    })
  }

  provider.receive("/mqtt", (response, error) => {
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
