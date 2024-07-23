import { useState } from 'react'
import { toast, Toaster } from 'react-hot-toast'
import type { AppProps } from 'next/app'
import useWebSocket from 'react-use-websocket'

function MyApp({ Component, pageProps, router} : AppProps) {
  return (
	<div>
    	<Component { ...pageProps } />
	<Toaster />
	</div>
  )
}
export default MyApp
