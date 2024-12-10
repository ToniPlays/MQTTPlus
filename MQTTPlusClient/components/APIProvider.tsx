import { createContext, useEffect, useRef } from "react"
import { MQTTPlusProvider } from "../app/client/mqttplus"
import { ReadyState } from "react-use-websocket"
import { WebSocketLike } from "react-use-websocket/dist/lib/types"

const APIProviderContext = createContext<WebSocketLike | null>(null);


export const APIProvider = ({ children }) => {
    const socketRef = useRef<WebSocketLike | null>(null)
    const provider = MQTTPlusProvider()

    useEffect(() => {
        if(provider.status == ReadyState.OPEN)
            socketRef.current = provider.socket.getWebSocket()

        return () => {
            socketRef.current?.close()
        }
    }, [provider.socket])

    return (
        <APIProviderContext.Provider value={socketRef.current}>
            {children}
        </APIProviderContext.Provider>
    )
}