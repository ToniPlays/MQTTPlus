

export function FormatPercentage(value: number | undefined | null) {
    if (!value) return 0;
    return (value * 100.0).toFixed(2);
}
export function FormatBytes(bytes: number | undefined | null) {
    if (!bytes) return "0 Bytes";
    const KB = 1000;
    const MB = KB * KB;
    const GB = MB * KB;
    

    if (bytes > GB) return `${(bytes / GB).toPrecision(4)} GB`;
    if (bytes > MB) return `${(bytes / MB).toPrecision(4)} MB`;
    if (bytes > KB) return `${(bytes / MB).toPrecision(4)} KB`;
    return `${bytes} Bytes`;
}
export function Capitalize(value: string)
{
    const f = value.charAt(0)
    if('0' <= f && f <= '9')
        return value
    return f.toUpperCase() + value.substring(1)
}