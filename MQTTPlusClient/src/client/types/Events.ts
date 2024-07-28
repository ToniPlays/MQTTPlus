
export module MQTTPlus {
    export class Events {
        public readonly endpoint = "/events"

        listen(opts: Events.ListenOpts) {
            return {
                endpoint: this.endpoint,
                opts: opts,
            }
        }
   }

   export module Events {
       export enum EventType {
           ServerStatus = 'server.status'
       }

       export interface ListenEventOpts {
          type: EventType
          interval?: number //Not applicable for every event
       }

       export interface ListenOpts {
           listen?: Array<ListenEventOpts>
       }
   }
}