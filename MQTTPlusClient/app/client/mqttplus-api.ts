
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
            id: id,
            opts: expand,
        }
    }

    networks(expand?: ExpandOpts)
    {
        return {
            endpoint: '/networks',
            opts: expand,
        }
    }

    network(id: string, expand?: ExpandOpts)
    {
        return {
            endpoint: '/network',
            id: id,
            opts: expand,
        }
    }

    server(expand?: ExpandOpts) {
        return {
            endpoint: '/server',
            opts: expand,
        }
    }
   
   event(data: { listen: string[] }) {
        return {
            endpoint: '/event',
            listen: data.listen,
        }
   }
}

