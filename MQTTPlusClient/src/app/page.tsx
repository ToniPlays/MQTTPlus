"use client";

import React from "react";
import DefaultLayout from "../components/DefaultLayout"
import TimeAgo from "javascript-time-ago";
import en from "javascript-time-ago/locale/en.json";

export default function App() {

  TimeAgo.addDefaultLocale(en)
  
  return (
    <>
      <DefaultLayout>
        <p></p>
      </DefaultLayout>
    </>
  );
}
