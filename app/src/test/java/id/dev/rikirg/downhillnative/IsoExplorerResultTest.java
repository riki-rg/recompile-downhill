package id.dev.rikirg.downhillnative;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import java.util.List;

public class IsoExplorerResultTest {
    @Test
    public void formatsAReadableBoundedReport() {
        IsoExplorerResult result = IsoExplorerResult.success(
                "DOWNHILL_USA", List.of("SYSTEM.CNF;1  |  64 B", "DATA.BIN;1  |  2.00 MiB"), false);

        assertTrue(result.isSuccess());
        assertEquals("ISO-9660 volume: DOWNHILL_USA\n\nRoot entries (2):\nSYSTEM.CNF;1  |  64 B\nDATA.BIN;1  |  2.00 MiB\n\nThis is an ISO structure report, not yet a Downhill Domination compatibility result.",
                result.toDisplayText());
    }

    @Test
    public void formatsTruncationNoticeWithoutClaimingCompatibility() {
        IsoExplorerResult result = IsoExplorerResult.success("UNTITLED", List.of("A.BIN;1  |  1 B"), true);

        assertTrue(result.toDisplayText().contains("showing the first entries only"));
        assertTrue(result.toDisplayText().contains("not yet a Downhill Domination compatibility result"));
    }

    @Test
    public void formatsFailureAsUserSafeMessage() {
        IsoExplorerResult result = IsoExplorerResult.failure("Could not read an ISO-9660 root directory.");

        assertEquals("Could not read an ISO-9660 root directory.", result.toDisplayText());
    }
}
