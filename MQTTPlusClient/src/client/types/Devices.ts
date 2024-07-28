
export module MQTTPlus {
    export class Devices {
        public readonly endpoint = "/devices"

        list(opts?: Devices.DeviceListOpts) {
            return {
                endpoint: this.endpoint,
                opts: opts,
            }
       }
   }

   export module Devices {
       export enum ExpandOpts {
        
       }

       export interface DeviceListOpts {
        network?: string
        expands?: Array<ExpandOpts>
       }
   }
}