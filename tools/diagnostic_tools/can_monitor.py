#!/usr/bin/env python3
"""
CAN Bus Monitor Tool
Monitor all CAN frames on the specified interface
"""

import sys
import subprocess

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <can_interface>")
        print(f"Example: {sys.argv[0]} can0")
        sys.exit(1)
    
    interface = sys.argv[1]
    
    # Check if interface exists
    result = subprocess.run(['ip', 'link', 'show', interface], 
                          capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: Interface {interface} not found")
        print("Available interfaces:")
        subprocess.run(['ip', '-br', 'link', 'show', 'type', 'can'])
        sys.exit(1)
    
    # Bring up interface if down
    subprocess.run(['sudo', 'ip', 'link', 'set', interface, 'up'], 
                   capture_output=True)
    
    print(f"Monitoring CAN frames on {interface}...")
    print("Press Ctrl+C to stop\n")
    print("Format: timestamp  interface  ID  DLC  Data")
    print("-" * 60)
    
    try:
        subprocess.run(['candump', '-ta', interface])
    except KeyboardInterrupt:
        print("\n\nMonitoring stopped")
    except FileNotFoundError:
        print("Error: candump not found. Install can-utils:")
        print("  sudo apt-get install can-utils")
        sys.exit(1)

if __name__ == "__main__":
    main()
