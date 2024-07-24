import React from 'react'
import type { AppProps } from 'next/app'

import '../styles/globals.css'
import { Toaster } from 'react-hot-toast'
import { CssBaseline, ThemeProvider, createTheme } from '@mui/material'

function MyApp({ Component, pageProps, router }: AppProps) {

  const theme = createTheme({
    palette: {
      mode: 'dark',
    },
  });

  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Component {...pageProps} />
      <Toaster />
    </ThemeProvider>
  )
}

export default MyApp
