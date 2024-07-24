import { useCallback, useEffect, useState } from "react"
import useWebSocket from "react-use-websocket"

export const url = "http://80.95.140.80:8884"


export const mqttPlusProvider = <T = unknown> () => {

    const socket = useWebSocket(url)
    const [receiveFuncs, setReceiveFuncs] = useState(new Map<string, (data: any, error: any | null) => void>())

    useEffect(() => {
        if(socket.lastJsonMessage == null) return;
        const message = socket.lastJsonMessage
        const type = message['endpoint']
        console.log(message)

        if(receiveFuncs.has(type))
            receiveFuncs.get(type)(socket.lastJsonMessage, null)

    }, [socket.lastJsonMessage])

    const postFunc = useCallback((message: any) => {
        socket.sendMessage(JSON.stringify(message))
    }, [])

    const receiveFunc = (address: string, callback: (data: any, error: any | null) => void)=> {
        if(receiveFuncs.has(address)) return

        const map = receiveFuncs
        map.set(address, callback)
        console.log("Set func")
        setReceiveFuncs(map)
    }

    return {
        status: socket.readyState,
        post: postFunc,
        receive: receiveFunc
    }
}