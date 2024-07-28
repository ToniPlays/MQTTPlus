
export module MQTTPlus {
     export class Server {
         public readonly endpoint = "/server"

         status(opts?: Server.ServerOpts) {
             return {
                 endpoint: this.endpoint,
                 opts: opts,
             }
        }
    }

    export module Server {
        export enum ExpandOpts {
            Services = 'services.info',
            Status = 'status',
        }

        export interface ServerOpts {
            expands?: Array<ExpandOpts>
        }
    }
}