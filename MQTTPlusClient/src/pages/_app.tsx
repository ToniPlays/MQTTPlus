import { useState } from 'react'
import toast from 'react-hot-toast'

function MyApp() {

  const [socket] = useState<WebSocket>(new WebSocket("ws://localhost:8886"))

  const [connectionStatus, setConnectionStatus] = useState<number>(WebSocket.CLOSED)

  InitializeWebSocket(socket)

  switch(connectionStatus)
  {
    case WebSocket.CLOSED:
      return (
        <p>Closed</p>
      )
    case WebSocket.CLOSING:
      return (
        <p>Closing</p>
      )
    case WebSocket.CONNECTING:
      return (
        <p>Connecting</p>
      )
    default: break;
  }

  return (
    <p>Fully going</p>
  )

  function InitializeWebSocket(socket: WebSocket)
  {
    setTimeout(() => {
      if(socket.CONNECTING)
        socket.close()
    }, 10000)

    socket.onopen = () => {
      setConnectionStatus(WebSocket.OPEN)
    }
    socket.onclose = () => {
      setConnectionStatus(WebSocket.CLOSED)
    }
    socket.onmessage = () => {
      socket.send("Wassup")
    }
    socket.onerror = (error) => {
      toast.error(`Error: ${error}`)
    }
  }
}
export default MyApp
