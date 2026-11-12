package id.dev.rikirg.downhillnative;

/** Checks for the ISO-9660 primary volume descriptor at sector 16. */
public final class IsoImageValidator {
    public static final int SECTOR_SIZE = 2048;

    private IsoImageValidator() {
    }

    public enum Result {
        VALID,
        NOT_ISO_9660,
        UNREADABLE
    }

    public static Result validateSector(byte[] sector) {
        if (sector == null || sector.length < SECTOR_SIZE) {
            return Result.UNREADABLE;
        }

        boolean signatureMatches = sector[0] == 1
                && sector[1] == 'C'
                && sector[2] == 'D'
                && sector[3] == '0'
                && sector[4] == '0'
                && sector[5] == '1'
                && sector[6] == 1;
        return signatureMatches ? Result.VALID : Result.NOT_ISO_9660;
    }
}
