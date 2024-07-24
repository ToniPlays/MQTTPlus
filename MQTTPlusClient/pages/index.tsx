import React, { useEffect, useState } from 'react'
import { Button } from '@mui/material'

import { NextPage } from 'next'
import toast from 'react-hot-toast'
import { mqttPlusProvider, url } from '../client/mqttplus'

interface Props {

};

const Home: NextPage<Props> = props => {

  const provider = mqttPlusProvider()
  const [message, setMessage] = useState("")

  function TestPost(message: string)
  {
    provider.post({
      request: "/test",
      message: 'My message'
    })
  }

  provider.receive("/test", (response, error) => {
    setMessage(response.message)
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
