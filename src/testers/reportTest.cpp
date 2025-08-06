#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include "../include/models.hpp"
#include "../include/reportModule.hpp"

// Utility function to display a separator line
void displaySeparator(char symbol = '-', int length = 50) {
    std::cout << std::string(length, symbol) << std::endl;
}

// Utility function to display a section header
void displayHeader(const std::string& title) {
    displaySeparator('=');
    std::cout << title << std::endl;
    displaySeparator('=');
}

// Detailed display of a single concert report
void displayConcertReport(const std::shared_ptr<Model::ConcertReport>& report, bool detailed = true) {
    if (!report) {
        std::cout << "[NULL REPORT REFERENCE]" << std::endl;
        return;
    }
    
    std::cout << "Report ID: " << report->id << std::endl;
    std::cout << "Date: " << report->date.iso8601String << std::endl;
    std::cout << "Total Registrations: " << report->total_registrations << std::endl;
    std::cout << "Tickets Sold: " << report->tickets_sold << std::endl;
    std::cout << "Sales Volume: $" << std::fixed << std::setprecision(2) << report->sales_volume << std::endl;
    
    if (detailed) {
        std::cout << "Attendee Engagement Score: " << std::fixed << std::setprecision(1) << report->attendee_engagement_score << std::endl;
        std::cout << "NPS Score: " << std::fixed << std::setprecision(1) << report->nps_score << std::endl;
        std::cout << "Created At: " << report->created_at.iso8601String << std::endl;
        std::cout << "Updated At: " << report->updated_at.iso8601String << std::endl;
        
        auto concert = report->concert.lock();
        if (concert) {
            std::cout << "Concert: " << concert->name << " (ID: " << concert->id << ")" << std::endl;
        } else {
            std::cout << "Concert: [Not linked]" << std::endl;
        }
    }
}

// Display a collection of reports
void displayReportList(const std::vector<std::shared_ptr<Model::ConcertReport>>& reports) {
    if (reports.empty()) {
        std::cout << "No reports found." << std::endl;
        return;
    }
    
    std::cout << "Found " << reports.size() << " report(s):" << std::endl;
    displaySeparator();
    
    // Table header
    std::cout << std::setw(8) << "ID"
              << std::setw(20) << "Date"
              << std::setw(12) << "Tickets Sold"
              << std::setw(15) << "Sales Volume"
              << std::setw(10) << "NPS Score" << std::endl;
    displaySeparator();
    
    // Table rows
    for (const auto& report : reports) {
        std::string shortDate = report->date.iso8601String.length() > 16 ?
                               report->date.iso8601String.substr(0, 16) :
                               report->date.iso8601String;
        
        std::cout << std::setw(8) << report->id
                  << std::setw(20) << shortDate
                  << std::setw(12) << report->tickets_sold
                  << std::setw(15) << std::fixed << std::setprecision(2) << report->sales_volume
                  << std::setw(10) << std::fixed << std::setprecision(1) << report->nps_score << std::endl;
    }
    displaySeparator();
}

// Test CREATE operation
std::vector<std::shared_ptr<Model::ConcertReport>> testCreateOperation(ReportManager::ReportModule& module) {
    displayHeader("CREATE OPERATION TEST");
    
    std::vector<std::shared_ptr<Model::ConcertReport>> createdReports;
    
    // Create report 1
    std::cout << "Creating report 1 (Concert 1 analytics)..." << std::endl;
    int reportId1 = module.generateConcertReport(1);
    if (reportId1 != -1) {
        auto report1 = module.getReportById(reportId1);
        if (report1) {
            createdReports.push_back(report1);
            displayConcertReport(report1);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create report 1" << std::endl;
    }
    
    // Create report 2
    std::cout << "Creating report 2 (Concert 2 analytics)..." << std::endl;
    int reportId2 = module.generateConcertReport(2);
    if (reportId2 != -1) {
        auto report2 = module.getReportById(reportId2);
        if (report2) {
            createdReports.push_back(report2);
            displayConcertReport(report2);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create report 2" << std::endl;
    }
    
    // Create report 3
    std::cout << "Creating report 3 (Concert 3 analytics)..." << std::endl;
    int reportId3 = module.generateConcertReport(3);
    if (reportId3 != -1) {
        auto report3 = module.getReportById(reportId3);
        if (report3) {
            createdReports.push_back(report3);
            displayConcertReport(report3);
            displaySeparator();
        }
    } else {
        std::cout << "Failed to create report 3" << std::endl;
    }
    
    return createdReports;
}

// Test READ operations
void testReadOperations(ReportManager::ReportModule& module, const std::vector<std::shared_ptr<Model::ConcertReport>>& testReports) {
    displayHeader("READ OPERATIONS TEST");
    
    // Get report by ID
    if (!testReports.empty()) {
        int idToFind = testReports[0]->id;
        std::cout << "1. Getting report by ID " << idToFind << ":" << std::endl;
        auto foundReport = module.getReportById(idToFind);
        if (foundReport) {
            displayConcertReport(foundReport);
        } else {
            std::cout << "Report not found!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Get reports by concert ID
    std::cout << "2. Getting reports for concert 1:" << std::endl;
    auto concertReports = module.getReportsByConcert(1);
    displayReportList(concertReports);
    std::cout << std::endl;
    
    // Get reports by date range
    std::cout << "3. Getting reports by date range (2024-01-01 to 2024-12-31):" << std::endl;
    auto dateRangeReports = module.getReportsByDateRange("2024-01-01", "2024-12-31");
    displayReportList(dateRangeReports);
    std::cout << std::endl;
    
    // Get latest report for a concert
    std::cout << "4. Getting latest report for concert 1:" << std::endl;
    auto latestReport = module.getLatestReportForConcert(1);
    if (latestReport) {
        displayConcertReport(latestReport);
    } else {
        std::cout << "No reports found for concert 1" << std::endl;
    }
    std::cout << std::endl;
    
    // Test not finding a report
    std::cout << "5. Testing retrieval of non-existent report (ID 9999):" << std::endl;
    auto notFound = module.getReportById(9999);
    if (notFound) {
        std::cout << "Unexpected: Found report with ID 9999" << std::endl;
    } else {
        std::cout << "Correctly returned nullptr for non-existent report" << std::endl;
    }
    std::cout << std::endl;
}

// Test UPDATE operations
void testUpdateOperations(ReportManager::ReportModule& module, const std::vector<std::shared_ptr<Model::ConcertReport>>& testReports) {
    displayHeader("UPDATE OPERATIONS TEST");
    
    if (testReports.empty()) {
        std::cout << "No reports available for update testing." << std::endl;
        return;
    }
    
    // Update first report
    auto reportToUpdate = testReports[0];
    std::cout << "1. Updating report ID " << reportToUpdate->id << ":" << std::endl;
    std::cout << "Before update:" << std::endl;
    displayConcertReport(reportToUpdate);
    
    bool updateResult = module.updateConcertReport(reportToUpdate->id);
    if (updateResult) {
        std::cout << "\nAfter update:" << std::endl;
        auto updatedReport = module.getReportById(reportToUpdate->id);
        if (updatedReport) {
            displayConcertReport(updatedReport);
        }
    } else {
        std::cout << "Update failed!" << std::endl;
    }
    std::cout << std::endl;
    
    // Test updating non-existent report
    std::cout << "2. Testing update of non-existent report (ID 8888):" << std::endl;
    bool failedUpdate = module.updateConcertReport(8888);
    if (!failedUpdate) {
        std::cout << "Correctly failed to update non-existent report" << std::endl;
    } else {
        std::cout << "Unexpected: Successfully updated non-existent report" << std::endl;
    }
    std::cout << std::endl;
}

// Test Analytics operations
void testAnalyticsOperations(ReportManager::ReportModule& module) {
    displayHeader("ANALYTICS OPERATIONS TEST");
    
    // Test concert metrics
    std::cout << "1. Getting concert metrics for concert 1:" << std::endl;
    auto metrics = module.getConcertMetrics(1);
    std::cout << "Concert ID: " << metrics.concert_id << std::endl;
    std::cout << "Concert Name: " << metrics.concert_name << std::endl;
    std::cout << "Total Registrations: " << metrics.total_registrations << std::endl;
    std::cout << "Tickets Sold: " << metrics.tickets_sold << std::endl;
    std::cout << "Sales Volume: $" << std::fixed << std::setprecision(2) << metrics.sales_volume << std::endl;
    std::cout << "Capacity Utilization: " << std::fixed << std::setprecision(1) << metrics.capacity_utilization << "%" << std::endl;
    std::cout << "Engagement Score: " << std::fixed << std::setprecision(1) << metrics.attendee_engagement_score << std::endl;
    std::cout << std::endl;
    
    // Test revenue breakdown
    std::cout << "2. Getting revenue breakdown (daily):" << std::endl;
    auto dailyRevenue = module.getRevenueBreakdown("2024-01-01", "2024-01-05", "daily");
    for (const auto& entry : dailyRevenue) {
        std::cout << entry.first << ": $" << std::fixed << std::setprecision(2) << entry.second << std::endl;
    }
    std::cout << std::endl;
    
    // Test attendance trends
    std::cout << "3. Getting attendance trends:" << std::endl;
    auto trends = module.getAttendanceTrends("2024-01-01", "2024-01-05");
    for (const auto& entry : trends) {
        std::cout << entry.first << ": " << entry.second << " attendees" << std::endl;
    }
    std::cout << std::endl;
}

// Test Report Generation operations
void testReportGeneration(ReportManager::ReportModule& module) {
    displayHeader("REPORT GENERATION TEST");
    
    // Test sales analytics report
    std::cout << "1. Generating sales analytics report:" << std::endl;
    std::string salesReport = module.generateSalesAnalyticsReport("2024-01-01", "2024-01-31", "JSON");
    if (!salesReport.empty()) {
        std::cout << "Sales analytics report generated successfully:" << std::endl;
        std::cout << salesReport.substr(0, 200) << "..." << std::endl; // Show first 200 chars
    } else {
        std::cout << "Failed to generate sales analytics report" << std::endl;
    }
    std::cout << std::endl;
    
    // Test payroll report
    std::cout << "2. Generating payroll report:" << std::endl;
    std::string payrollReport = module.generatePayrollReport("2024-01-01", "2024-01-31", "CSV");
    if (!payrollReport.empty()) {
        std::cout << "Payroll report generated successfully:" << std::endl;
        std::cout << payrollReport.substr(0, 200) << "..." << std::endl; // Show first 200 chars
    } else {
        std::cout << "Failed to generate payroll report" << std::endl;
    }
    std::cout << std::endl;
}

// Main test function
int main() {
    displayHeader("REPORT MODULE COMPREHENSIVE TEST");
    
    try {
        // Initialize the module
        ReportManager::ReportModule reportModule("test_reports.dat");
        std::cout << "ReportModule initialized successfully!" << std::endl;
        displaySeparator();
        
        // Test CREATE operations
        auto testReports = testCreateOperation(reportModule);
        
        // Test READ operations
        testReadOperations(reportModule, testReports);
        
        // Test UPDATE operations
        testUpdateOperations(reportModule, testReports);
        
        // Test Analytics operations
        testAnalyticsOperations(reportModule);
        
        // Test Report Generation operations
        testReportGeneration(reportModule);
        
        displayHeader("ALL TESTS COMPLETED SUCCESSFULLY");
        std::cout << "Report Module testing completed without critical errors." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during testing: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught during testing." << std::endl;
        return 1;
    }
    
    return 0;
}
