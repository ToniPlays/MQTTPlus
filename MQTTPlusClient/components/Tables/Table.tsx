"use client";

import React from "react";

interface TableProps {
  label: string;
  columns: string[]
  children: React.ReactNode
}

const Table = (props: TableProps) => {
  return (
    <div className="rounded-sm border border-stroke bg-white px-5 pb-2.5 pt-6 shadow-default dark:border-strokedark dark:bg-boxdark sm:px-7.5 xl:pb-1">
      <h4 className="mb-6 text-xl font-semibold text-black dark:text-white">
        {props.label}
      </h4>

      <div className="flex flex-col">
        <div className={`grid rounded-sm bg-gray-2 dark:bg-meta-4`} style={{ gridTemplateColumns: 'auto auto auto auto' }}>
          {props.columns.map((column) => {
            return (
              <div className="p-2.5 xl:p-5" key={column}>
                <h5 className="text-sm font-medium uppercase xsm:text-base">
                  {column}
                </h5>
              </div>
            );
          })}
        </div>
        {props.children}
      </div>
    </div>
  );
};

export default Table;
