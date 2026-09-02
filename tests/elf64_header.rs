use mini_elf_toolchain::elf64::{Elf64Header, ElfError, ELF64_HEADER_SIZE};

fn base_header() -> Vec<u8> {
    let mut bytes = vec![0u8; ELF64_HEADER_SIZE];
    bytes[0..4].copy_from_slice(&[0x7f, b'E', b'L', b'F']);
    bytes[4] = 2;
    bytes[5] = 1;
    bytes[6] = 1;
    bytes[16..18].copy_from_slice(&1u16.to_le_bytes());
    bytes[18..20].copy_from_slice(&62u16.to_le_bytes());
    bytes[20..24].copy_from_slice(&1u32.to_le_bytes());
    bytes[52..54].copy_from_slice(&(ELF64_HEADER_SIZE as u16).to_le_bytes());
    bytes
}

#[test]
fn parses_minimal_relocatable_header() {
    let header = Elf64Header::parse(&base_header()).unwrap();
    assert_eq!(header.elf_type, 1);
    assert_eq!(header.machine, 62);
    assert_eq!(header.program_header_count, 0);
    assert_eq!(header.section_header_count, 0);
}

#[test]
fn rejects_truncated_header() {
    assert_eq!(
        Elf64Header::parse(&[0u8; 8]),
        Err(ElfError::TruncatedHeader { actual: 8 })
    );
}

#[test]
fn rejects_wrong_identity_and_machine() {
    let mut bad_magic = base_header();
    bad_magic[0] = 0;
    assert_eq!(Elf64Header::parse(&bad_magic), Err(ElfError::BadMagic));

    let mut wrong_class = base_header();
    wrong_class[4] = 1;
    assert_eq!(
        Elf64Header::parse(&wrong_class),
        Err(ElfError::UnsupportedClass(1))
    );

    let mut big_endian = base_header();
    big_endian[5] = 2;
    assert_eq!(
        Elf64Header::parse(&big_endian),
        Err(ElfError::UnsupportedEndianness(2))
    );

    let mut wrong_machine = base_header();
    wrong_machine[18..20].copy_from_slice(&3u16.to_le_bytes());
    assert_eq!(
        Elf64Header::parse(&wrong_machine),
        Err(ElfError::UnsupportedMachine(3))
    );
}

#[test]
fn rejects_bad_entry_sizes_when_tables_are_present() {
    let mut ph = base_header();
    ph.resize(120, 0);
    ph[32..40].copy_from_slice(&64u64.to_le_bytes());
    ph[54..56].copy_from_slice(&8u16.to_le_bytes());
    ph[56..58].copy_from_slice(&1u16.to_le_bytes());
    assert_eq!(
        Elf64Header::parse(&ph),
        Err(ElfError::InvalidProgramHeaderEntrySize(8))
    );

    let mut sh = base_header();
    sh.resize(128, 0);
    sh[40..48].copy_from_slice(&64u64.to_le_bytes());
    sh[58..60].copy_from_slice(&8u16.to_le_bytes());
    sh[60..62].copy_from_slice(&1u16.to_le_bytes());
    assert_eq!(
        Elf64Header::parse(&sh),
        Err(ElfError::InvalidSectionHeaderEntrySize(8))
    );
}

#[test]
fn rejects_out_of_bounds_program_header_table() {
    let mut bytes = base_header();
    bytes.resize(100, 0);
    bytes[32..40].copy_from_slice(&64u64.to_le_bytes());
    bytes[54..56].copy_from_slice(&56u16.to_le_bytes());
    bytes[56..58].copy_from_slice(&1u16.to_le_bytes());

    assert_eq!(
        Elf64Header::parse(&bytes),
        Err(ElfError::TableOutOfBounds {
            table: "program-header",
            end: 120,
            file_len: 100,
        })
    );
}

#[test]
fn rejects_out_of_bounds_section_header_table() {
    let mut bytes = base_header();
    bytes.resize(100, 0);
    bytes[40..48].copy_from_slice(&64u64.to_le_bytes());
    bytes[58..60].copy_from_slice(&64u16.to_le_bytes());
    bytes[60..62].copy_from_slice(&1u16.to_le_bytes());

    assert_eq!(
        Elf64Header::parse(&bytes),
        Err(ElfError::TableOutOfBounds {
            table: "section-header",
            end: 128,
            file_len: 100,
        })
    );
}

#[test]
fn rejects_table_range_overflow_before_bounds_check() {
    let mut bytes = base_header();
    bytes[32..40].copy_from_slice(&(u64::MAX - 10).to_le_bytes());
    bytes[54..56].copy_from_slice(&56u16.to_le_bytes());
    bytes[56..58].copy_from_slice(&1u16.to_le_bytes());

    assert_eq!(
        Elf64Header::parse(&bytes),
        Err(ElfError::TableRangeOverflow("program-header"))
    );
}
