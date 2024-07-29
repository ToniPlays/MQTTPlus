
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
           ServerStatus = 'server.status',
           MQTTClientConnectionStatusChanged = 'mqtt.client_connection_status_changed',
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