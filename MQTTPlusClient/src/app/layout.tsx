"use client";

import "jsvectormap/dist/jsvectormap.css"
import "flatpickr/dist/flatpickr.min.css"
import "@/css/satoshi.css"
import "@/css/style.css"
import React, { useState } from "react"
import { MQTTPlusProvider } from "../client/mqttplus";

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  const [sidebarOpen, setSidebarOpen] = useState(false);
  const [loading, setLoading] = useState<boolean>(true);

  const provider = MQTTPlusProvider()

  return (
    <html lang="en">
      <body suppressHydrationWarning={true} className="dark:text-bodydark">
        <div className="dark:bg-boxdark-2 dark:text-bodydark" style={{ minHeight: '100vh'}}>
          {children}
        </div>
      </body>
    </html>
  );
}
