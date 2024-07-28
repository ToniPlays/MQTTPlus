
export const KB = 1000;
export const MB = KB * KB;
export const GB = MB * KB;

export function FormatPercentage(value: number | undefined | null) {
    if (!value) return 0;
    return (value * 100.0).toFixed();
}
export function FormatBytes(bytes: number | undefined | null) {
    if (!bytes) return "0 Bytes";
    
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