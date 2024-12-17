import { APIProvider } from "@/APIProvider"
import DefaultLayout from "@/DefaultLayout"
import TimeAgo from "javascript-time-ago";
import en from "javascript-time-ago/locale/en";
import { AppProps } from "next/app"
import toast, { Toaster } from "react-hot-toast"
import { MQTTPlusProvider } from "../app/client/mqttplus";
import { useEffect } from "react";
import { ReadyState } from "react-use-websocket";
import useColorMode from "../app/hooks/useColorMode";

TimeAgo.addDefaultLocale(en);

function MQTTPlusClientApp({ Component, pageProps, router }: AppProps) {

    const provider = MQTTPlusProvider()
    const api = provider.api

    useEffect(() => {
        if(provider.status == ReadyState.OPEN)
            toast.success("Connected")
    }, [provider.status])

    return (
        <APIProvider>
            <DefaultLayout>
                <Component {...pageProps} />
            </DefaultLayout>
            <Toaster />
        </APIProvider>
    )
}

export default MQTTPlusClientApp