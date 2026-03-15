# UDS Configuration Integration Guide

## Generated Files

- `UdsConfig_Generated.h` - Header file with all UDS configuration macros
- `UdsConfig_Generated.c` - Implementation with data tables and functions

## Quick Integration

### 1. Copy Files

```bash
cp generated/* $YOUR_PROJECT/AUTOSAR/UdsConfig/
```

### 2. Update Makefile

Add the generated source file to your build:

```makefile
# In your Makefile
C_SOURCES += AUTOSAR/UdsConfig/UdsConfig_Generated.c
C_INCLUDES += -IAUTOSAR/UdsConfig
```

### 3. Update DCM Configuration

Modify your `AUTOSAR_Cfg.h` to use generated configurations:

```c
#include "UdsConfig_Generated.h"

/* Replace static DCM config with generated ones */
#define DCM_NUM_DIDS    (sizeof(Uds_DidTable)/sizeof(Uds_DidTable[0]))
#define DCM_NUM_DTCS    (sizeof(Uds_DtcTable)/sizeof(Uds_DtcTable[0]))

/* Use generated read/write functions */
#define DCM_READ_DID_FUNC   UdsConfig_ReadDid
#define DCM_WRITE_DID_FUNC  UdsConfig_WriteDid
```

### 4. Initialize

Call initialization in your main:

```c
#include "UdsConfig_Generated.h"

void main(void) {
    /* ... other init ... */
    UdsConfig_Init();
    Dcm_Init(&Dcm_Config);
    /* ... */
}
```

## Configuration Summary

| Parameter | Value |
|-----------|-------|
| Functional Request ID | 0x7df |
| Physical Request ID | 0x735 |
| Response ID | 0x73d |
| Sessions | 3 |
| DIDs | 8 |
| DTCs | 5 |

## Regenerating Code

After modifying `UDS_Config_Example.xlsx`:

```bash
python3 tools/uds_generator/uds_generator.py UDS_Config_Example.xlsx generated
```
