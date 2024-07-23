import type { NextPage} from 'next'
import { toast } from 'react-hot-toast'
import { url } from '../client.ts'
import useWebSocket, { ReadyState } from 'react-use-websocket'
import React, { useEffect, useState } from 'react'

interface Props
{
}

const Login: NextPage<Props> = props => {
    
	const { sendMessage, lastJsonMessage, readyState} = useWebSocket(url)
    
	const [state, setState] = useState<ReadyState>(readyState)

	useEffect(() => {
		console.log("Changed")
		console.log(readyState)
		setState(readyState)
	}, [readyState])

	const connectionStatus = {
		[ReadyState.CONNECTING]: 'Connecting',
		[ReadyState.OPEN]: 'Open',
		[ReadyState.CLOSING]: 'Closing',
		[ReadyState.CLOSED]: 'Closed',
		[ReadyState.UNINSTANTIATED]: 'Uninstantiated',
	  }[state];

    return (
	<div>
		<p>Login page</p>
		<p>WebSocket is: {connectionStatus}</p>
		<button onClick={() => toast.success(`Socket status: ${readyState.toString()}`) }>Check state</button>
    		
	</div>
    )
}

export default Login