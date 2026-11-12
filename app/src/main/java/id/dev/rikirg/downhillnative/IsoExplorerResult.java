package id.dev.rikirg.downhillnative;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/** Safe, metadata-only output from the native ISO explorer. */
public final class IsoExplorerResult {
    private final String volumeId;
    private final List<String> entries;
    private final boolean truncated;
    private final String error;

    private IsoExplorerResult(String volumeId, List<String> entries, boolean truncated, String error) {
        this.volumeId = volumeId;
        this.entries = Collections.unmodifiableList(new ArrayList<>(entries));
        this.truncated = truncated;
        this.error = error;
    }

    public static IsoExplorerResult success(String volumeId, List<String> entries, boolean truncated) {
        return new IsoExplorerResult(volumeId, entries, truncated, "");
    }

    public static IsoExplorerResult failure(String message) {
        return new IsoExplorerResult("", List.of(), false, message);
    }

    public boolean isSuccess() {
        return error.isEmpty();
    }

    public String toDisplayText() {
        if (!isSuccess()) {
            return error;
        }
        StringBuilder report = new StringBuilder("ISO-9660 volume: ")
                .append(volumeId)
                .append("\n\nRoot entries (")
                .append(entries.size())
                .append("):");
        for (String entry : entries) {
            report.append('\n').append(entry);
        }
        if (truncated) {
            report.append("\n\nDirectory is large; showing the first entries only.");
        }
        report.append("\n\nThis is an ISO structure report, not yet a Downhill Domination compatibility result.");
        return report.toString();
    }
}
