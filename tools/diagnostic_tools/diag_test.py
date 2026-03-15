#!/usr/bin/env python3
"""
UDS Diagnostic Test Tool
Test diagnostic services using DBC matrix CAN IDs
- RX: 0x7DF (Meg_FunDiag), 0x735 (Meg_PhyDiag)
- TX: 0x73D (Meg_ResDiag)
"""

import sys
import subprocess
import time
import struct

def send_can_frame(interface, can_id, data):
    """Send a CAN frame using cansend"""
    hex_data = '.'.join(f'{b:02X}' for b in data)
    cmd = ['cansend', interface, f'{can_id:03X}#{hex_data}']
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode == 0

def decode_service(service_id):
    """Decode UDS service ID to name"""
    services = {
        0x10: "DiagnosticSessionControl",
        0x11: "ECUReset",
        0x14: "ClearDiagnosticInformation",
        0x19: "ReadDTCInformation",
        0x22: "ReadDataByIdentifier",
        0x27: "SecurityAccess",
        0x2E: "WriteDataByIdentifier",
        0x31: "RoutineControl",
        0x3E: "TesterPresent",
        0x50: "DiagnosticSessionControl (Response)",
        0x51: "ECUReset (Response)",
        0x62: "ReadDataByIdentifier (Response)",
        0x67: "SecurityAccess (Response)",
        0x6E: "WriteDataByIdentifier (Response)",
        0x7E: "TesterPresent (Response)",
        0x7F: "NegativeResponse"
    }
    return services.get(service_id, f"Unknown (0x{service_id:02X})")

def test_diagnostic_session_control(interface):
    """Test 0x10 - DiagnosticSessionControl"""
    print("\n[TEST] 0x10 - DiagnosticSessionControl")
    print("-" * 50)
    
    # Test Default Session (0x01)
    print("Sending: 10 01 (Default Session)")
    send_can_frame(interface, 0x7DF, [0x02, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)
    
    # Test Programming Session (0x02)
    print("Sending: 10 02 (Programming Session)")
    send_can_frame(interface, 0x7DF, [0x02, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)
    
    # Test Extended Session (0x03)
    print("Sending: 10 03 (Extended Session)")
    send_can_frame(interface, 0x7DF, [0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)

def test_ecu_reset(interface):
    """Test 0x11 - ECUReset"""
    print("\n[TEST] 0x11 - ECUReset")
    print("-" * 50)
    
    print("Sending: 11 01 (Hard Reset)")
    send_can_frame(interface, 0x7DF, [0x02, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)

def test_read_did(interface):
    """Test 0x22 - ReadDataByIdentifier"""
    print("\n[TEST] 0x22 - ReadDataByIdentifier")
    print("-" * 50)
    
    # Read VIN (0xF190)
    print("Sending: 22 F1 90 (Read VIN)")
    send_can_frame(interface, 0x7DF, [0x03, 0x22, 0xF1, 0x90, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.2)
    
    # Read ECU Name (0xF197)
    print("Sending: 22 F1 97 (Read ECU Name)")
    send_can_frame(interface, 0x7DF, [0x03, 0x22, 0xF1, 0x97, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.2)
    
    # Read System State (0xF100)
    print("Sending: 22 F1 00 (Read System State)")
    send_can_frame(interface, 0x7DF, [0x03, 0x22, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.2)

def test_tester_present(interface):
    """Test 0x3E - TesterPresent"""
    print("\n[TEST] 0x3E - TesterPresent")
    print("-" * 50)
    
    print("Sending: 3E 00 (Tester Present, No Response)")
    send_can_frame(interface, 0x7DF, [0x02, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)
    
    print("Sending: 3E 80 (Tester Present, With Response)")
    send_can_frame(interface, 0x7DF, [0x02, 0x3E, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.1)

def test_phy_diag_id(interface):
    """Test 0x735 Physical Diag ID"""
    print("\n[TEST] 0x735 - Physical Diag Request")
    print("-" * 50)
    
    print("Sending: 22 F1 90 on 0x735 (Physical Request)")
    send_can_frame(interface, 0x735, [0x03, 0x22, 0xF1, 0x90, 0x00, 0x00, 0x00, 0x00])
    time.sleep(0.2)

def print_dbc_matrix():
    """Print DBC matrix info"""
    print("\n" + "=" * 60)
    print("DBC Matrix - Diagnostic IDs")
    print("=" * 60)
    print("ID      | Name           | Direction | Description")
    print("-" * 60)
    print("0x7DF   | Meg_FunDiag    | RX        | Functional Diag Request")
    print("0x735   | Meg_PhyDiag    | RX        | Physical Diag Request")
    print("0x73D   | Meg_ResDiag    | TX        | Diag Response")
    print("=" * 60)

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <can_interface>")
        print(f"Example: {sys.argv[0]} can0")
        sys.exit(1)
    
    interface = sys.argv[1]
    
    # Check cansend
    if subprocess.run(['which', 'cansend'], capture_output=True).returncode != 0:
        print("Error: cansend not found. Install can-utils:")
        print("  sudo apt-get install can-utils")
        sys.exit(1)
    
    print_dbc_matrix()
    
    print("\nStarting UDS Diagnostic Tests...")
    print("Make sure candump is running in another terminal:")
    print(f"  candump -ta {interface}")
    print("\nPress Enter to start tests...")
    input()
    
    # Run all tests
    test_diagnostic_session_control(interface)
    test_ecu_reset(interface)
    test_read_did(interface)
    test_tester_present(interface)
    test_phy_diag_id(interface)
    
    print("\n" + "=" * 60)
    print("All diagnostic tests completed!")
    print("=" * 60)
    print("\nExpected Response IDs: 0x73D (Meg_ResDiag)")
    print("\nService Response Codes:")
    print("  0x50 = Session Control Response")
    print("  0x51 = ECU Reset Response")
    print("  0x62 = Read DID Response")
    print("  0x7E = Tester Present Response")
    print("  0x7F = Negative Response")

if __name__ == "__main__":
    main()
