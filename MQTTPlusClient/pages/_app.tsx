import { APIProvider } from "@/APIProvider"
import DefaultLayout from "@/DefaultLayout"
import { AppProps } from "next/app"
import { Toaster } from "react-hot-toast"

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