package id.dev.rikirg.downhillnative;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

public class IsoImageValidatorTest {
    @Test
    public void recognizesValidIso9660PrimaryVolumeDescriptor() {
        byte[] sector = new byte[IsoImageValidator.SECTOR_SIZE];
        sector[0] = 1;
        sector[1] = 'C';
        sector[2] = 'D';
        sector[3] = '0';
        sector[4] = '0';
        sector[5] = '1';
        sector[6] = 1;

        assertEquals(IsoImageValidator.Result.VALID, IsoImageValidator.validateSector(sector));
    }

    @Test
    public void rejectsSectorWithoutIso9660Signature() {
        byte[] sector = new byte[IsoImageValidator.SECTOR_SIZE];

        assertEquals(IsoImageValidator.Result.NOT_ISO_9660, IsoImageValidator.validateSector(sector));
    }

    @Test
    public void rejectsTruncatedSector() {
        assertEquals(IsoImageValidator.Result.UNREADABLE, IsoImageValidator.validateSector(new byte[10]));
    }
}
