import { APIProvider } from "@/APIProvider"
import DefaultLayout from "@/DefaultLayout"
import TimeAgo from "javascript-time-ago";
import en from "javascript-time-ago/locale/en";
import { AppProps } from "next/app"
import { Toaster } from "react-hot-toast"

TimeAgo.addDefaultLocale(en);

function MQTTPlusClientApp({ Component, pageProps, router }: AppProps) {

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