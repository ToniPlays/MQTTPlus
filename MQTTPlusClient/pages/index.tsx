import "jsvectormap/dist/jsvectormap.css"
import "flatpickr/dist/flatpickr.min.css"
import "../styles/satoshi.css"
import "../styles/global.css"

import React from "react"

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <>
      {children}
    </>
  )
}
