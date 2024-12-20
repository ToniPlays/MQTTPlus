export const KB = 1000;
export const MB = KB * KB;
export const GB = MB * KB;

export function FormatPercentage(value: number | undefined | null, precision: number = 2) {
    if (!value) return 0;
    return (value * 100.0).toFixed(precision);
}

export function FormatBytes(bytes: number | undefined | null) {
    if (!bytes) return "---";
    
    if (bytes > GB) return `${(bytes / GB).toFixed(2)} GB`;
    if (bytes > MB) return `${(bytes / MB).toFixed(2)} MB`;
    if (bytes > KB) return `${(bytes / MB).toFixed(2)} KB`;
    return `${bytes} Bytes`;
}

export function Capitalize(value: string)
{
    const f = value.charAt(0)
    if('0' <= f && f <= '9')
        return value
    return f.toUpperCase() + value.substring(1)
}

export function GetDeviceName(device: any)
{
    return device.nickname?.length > 0 ? device.nickname : device.client_id
}

export function GetId(object: any)
{
    return typeof object == "string" ? object : object.id
}