"use client"

import { useCallback, useEffect, useState } from "react"
import useWebSocket from "react-use-websocket"
import MQTTPlusAPI from "./mqttplus-api"


export const url = `${process.env.NEXT_PUBLIC_HOST}:8884`

/*This API is asynchronous, meaning a post message will not return anything,
unless provicer.receive(on, callbac) is set
This will allow for realtime data to be sent via websockets without any request
*/

export const MQTTPlusProvider = () => {
    const api = new MQTTPlusAPI()
    const socket = useWebSocket(url, {
        shouldReconnect: (e) => true,
        reconnectAttempts: 30,
        reconnectInterval: 5000,
        share: true,
    })

    const [receiveFuncs, setReceiveFuncs] = useState(new Map<string, (data: any, error: any | null) => void>())

    useEffect(() => {
        if(socket.lastJsonMessage == null) return;
        
        const message = socket.lastJsonMessage
        const type = message.endpoint

        if(receiveFuncs.has(type))
            receiveFuncs.get(type)!(socket.lastJsonMessage, null)

    }, [socket.lastJsonMessage])

    const postFunc = useCallback((message: any) => {
        socket.sendMessage(JSON.stringify(message))
    }, [])

    const receiveFunc = (address: string, callback: (data: any, error: any | null) => void)=> {
        if(receiveFuncs.has(address)) return

        const map = receiveFuncs
        map.set(address, callback)
        setReceiveFuncs(map)
    }

    return {
        status: socket.readyState,
        post: postFunc,
        receive: receiveFunc,
        api
    }
}