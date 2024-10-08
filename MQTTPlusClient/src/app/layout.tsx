"use client";

import "jsvectormap/dist/jsvectormap.css"
import "flatpickr/dist/flatpickr.min.css"
import "@/css/satoshi.css"
import "@/css/style.css"
import React, { useState } from "react"
import { MQTTPlusProvider } from "../client/mqttplus";
import { Toaster } from "react-hot-toast";

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  
  const provider = MQTTPlusProvider()

  return (
    <html lang="en">
      <body suppressHydrationWarning={true} className="dark:text-bodydark">
        <div className="dark:bg-boxdark-2 dark:text-bodydark" style={{ minHeight: '100vh'}}>
          {children}
          <Toaster />
        </div>
      </body>
    </html>
  );
}
