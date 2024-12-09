
export interface ExpandOpts {
    expands?: Array<string>
}

export default class MQTTPlusAPI {
   
    devices(expand?: ExpandOpts)
    {
        return {
            endpoint: '/devices',
            opts: expand,
        }
    }

    device(id: string, expand?: ExpandOpts)
    {
        return {
            endpoint: '/device',
            public_id: id,
            opts: expand,
        }
    }

    server(expand?: ExpandOpts) {
        return {
            endpoint: '/server',
            opts: expand,
        }
   }
   
   event(data: { listen: string[] })
   {
    return {
        endpoint: '/event',
        listen: data.listen,
    }
   }
}

