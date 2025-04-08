#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

// Global map to store prefix to AS mappings
map<string, set<int>> prefixToAS;

// Mock function to simulate RPKI validation (in real case, query an RPKI API)
bool validateWithRPKI(const string& prefix, int asn) {
    // Simulating a valid AS
    return (asn % 2 == 0);  // Example: Even ASNs are "valid"
}

// Function to detect MOAS conflicts
void detectMOAS(const string& prefix, int originAS) {
    prefixToAS[prefix].insert(originAS);
    
    if (prefixToAS[prefix].size() > 1) {
        cout << "MOAS Conflict detected for Prefix: " << prefix << " | Origin ASes: ";
        for (int asn : prefixToAS[prefix]) {
            cout << "AS" << asn << " ";
        }
        cout << endl;
    }
}

// Function to resolve MOAS conflicts
void resolveConflict(const string& prefix) {
    if (prefixToAS[prefix].size() <= 1) return;
    
    // Select the lowest AS number as the preferred one (as an example policy)
    int preferredAS = *min_element(prefixToAS[prefix].begin(), prefixToAS[prefix].end());
    
    cout << "Resolving MOAS Conflict: Preferring AS" << preferredAS << " for " << prefix << endl;
    
    // Additional policy: Remove invalid ASes based on RPKI validation
    set<int> validASes;
    for (int asn : prefixToAS[prefix]) {
        if (validateWithRPKI(prefix, asn)) {
            validASes.insert(asn);
        }
    }
    
    if (!validASes.empty()) {
        prefixToAS[prefix] = validASes;
        cout << "After RPKI Validation: Keeping " << preferredAS << " for " << prefix << endl;
    }
}

int main() {
    // Simulating incoming BGP updates
    vector<pair<string, int>> bgpUpdates = {
        {"192.168.1.0/24", 64512},
        {"192.168.1.0/24", 64513},  // Conflict occurs
        {"10.0.0.0/8", 64514},
        {"10.0.0.0/8", 64516},  // Another conflict
        {"172.16.0.0/16", 64518} // No conflict
    };
    
    // Processing BGP updates
    for (const auto& update : bgpUpdates) {
        detectMOAS(update.first, update.second);
    }
    
    // Resolving conflicts
    for (const auto& entry : prefixToAS) {
        resolveConflict(entry.first);
    }
    
    return 0;
}
