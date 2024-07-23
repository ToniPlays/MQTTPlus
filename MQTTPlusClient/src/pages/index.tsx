import type { NextPage} from 'next'
import { toast } from 'react-hot-toast'
import { useCallback } from 'react'
import client, { url } from '../client.ts'
import useWebSocket from 'react-use-websocket'

interface Props
{
}

const Login: NextPage<Props> = props => {
    
	const socket = useWebSocket(url)
    
    return (
	<div>
		<p>Login page</p>
		<button onClick={() => toast.success(socket.readyState) }>Check state</button>
    		
	</div>
    )
}

export default Login