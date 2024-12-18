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
        shouldReconnect: (e) => { return true; },
        reconnectAttempts: 30,
        reconnectInterval: 5000,
        share: true,
    })

    type rxCallback = (data: any, error: any | null) => void

    const [rxFunc, setRxFunc] = useState<Map<number, rxCallback | null>>(new Map());

    useEffect(() => {
        if(socket.lastJsonMessage == null) return;
        
        const message = socket.lastJsonMessage
        if(rxFunc.size == 0) return;

        rxFunc.get(0)!(message, null)

    }, [socket.lastJsonMessage])

    useEffect(() => {
        return () => {
            if(socket)
                socket.getWebSocket()?.close()
        }
    }, [socket.getWebSocket])

    const postFunc = useCallback((message: any) => {
        socket.sendMessage(JSON.stringify(message))
    }, [])

    const receiveFunc = (callback: rxCallback) => {
        const map = rxFunc
        map.set(0, callback)
        setRxFunc(map)
    }

    return {
        status: socket.readyState,
        post: postFunc,
        receive: receiveFunc,
        api,
        socket: socket
    }
}