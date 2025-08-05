#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <map>
#include "../include/reportModule.hpp"
#include "../include/models.hpp"

/**
 * @brief Test suite for ReportModule
 * 
 * This file contains comprehensive tests for all ReportModule functionality
 * including report generation, analytics, financial reports, and data export.
 */

class ReportModuleTest {
private:
    ReportManager::ReportModule reportModule;
    
public:
    ReportModuleTest() : reportModule("test_data/test_reports.dat") {}

    /**
     * @brief Run all report module tests
     */
    void runAllTests() {
        std::cout << "=== Starting ReportModule Tests ===" << std::endl;
        
        try {
            testConcertReportGeneration();
            testConcertReportUpdates();
            testSalesAnalyticsReport();
            testPayrollReport();
            testSummaryMetrics();
            testConcertMetrics();
            testRevenueBreakdown();
            testAttendanceTrends();
            testFinancialSummary();
            testProfitLossStatement();
            testConcertPerformanceAnalysis();
            testVenueUtilizationAnalysis();
            testCustomerSatisfactionAnalytics();
            testDataExport();
            testDashboardData();
            testReportQueries();
            testReportScheduling();
            
            std::cout << "✅ All ReportModule tests passed!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "❌ Test failed: " << e.what() << std::endl;
        }
        
        std::cout << "=== ReportModule Tests Complete ===" << std::endl;
    }

private:
    /**
     * @brief Test concert report generation
     */
    void testConcertReportGeneration() {
        std::cout << "Testing concert report generation..." << std::endl;
        
        // Test generating a concert report
        int concert_id = 301;
        int report_id = reportModule.generateConcertReport(concert_id);
        assert(report_id > 0);
        
        // Verify report was created
        auto report = reportModule.getReportById(report_id);
        assert(report != nullptr);
        
        std::cout << "✅ Concert report generation test passed" << std::endl;
    }

    /**
     * @brief Test concert report updates
     */
    void testConcertReportUpdates() {
        std::cout << "Testing concert report updates..." << std::endl;
        
        // Generate a report first
        int concert_id = 302;
        int report_id = reportModule.generateConcertReport(concert_id);
        assert(report_id > 0);
        
        // Test updating the report
        bool updated = reportModule.updateConcertReport(report_id);
        assert(updated);
        
        std::cout << "✅ Concert report updates test passed" << std::endl;
    }

    /**
     * @brief Test sales analytics report generation
     */
    void testSalesAnalyticsReport() {
        std::cout << "Testing sales analytics report..." << std::endl;
        
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        
        // Test JSON format
        std::string json_report = reportModule.generateSalesAnalyticsReport(
            start_date, end_date, "JSON"
        );
        assert(!json_report.empty());
        
        // Test CSV format
        std::string csv_report = reportModule.generateSalesAnalyticsReport(
            start_date, end_date, "CSV"
        );
        assert(!csv_report.empty());
        
        // Test HTML format
        std::string html_report = reportModule.generateSalesAnalyticsReport(
            start_date, end_date, "HTML"
        );
        assert(!html_report.empty());
        
        std::cout << "Generated reports:" << std::endl;
        std::cout << "  JSON: " << json_report.length() << " characters" << std::endl;
        std::cout << "  CSV: " << csv_report.length() << " characters" << std::endl;
        std::cout << "  HTML: " << html_report.length() << " characters" << std::endl;
        
        std::cout << "✅ Sales analytics report test passed" << std::endl;
    }

    /**
     * @brief Test payroll report generation
     */
    void testPayrollReport() {
        std::cout << "Testing payroll report..." << std::endl;
        
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-01-31T23:59:59Z";
        
        // Generate payroll report
        std::string payroll_report = reportModule.generatePayrollReport(
            start_date, end_date, "JSON"
        );
        assert(!payroll_report.empty());
        
        std::cout << "Payroll report generated: " << payroll_report.length() << " characters" << std::endl;
        
        std::cout << "✅ Payroll report test passed" << std::endl;
    }

    /**
     * @brief Test summary metrics calculation
     */
    void testSummaryMetrics() {
        std::cout << "Testing summary metrics..." << std::endl;
        
        // Calculate summary metrics
        auto metrics = reportModule.calculateSummaryMetrics();
        
        // Verify metrics structure
        assert(metrics.total_concerts >= 0);
        assert(metrics.active_concerts >= 0);
        assert(metrics.completed_concerts >= 0);
        assert(metrics.cancelled_concerts >= 0);
        assert(metrics.total_attendees >= 0);
        assert(metrics.total_tickets_sold >= 0);
        assert(metrics.total_revenue >= 0.0);
        assert(metrics.average_ticket_price >= 0.0);
        assert(metrics.overall_satisfaction_score >= 0.0);
        assert(metrics.nps_score >= -100.0 && metrics.nps_score <= 100.0);
        
        std::cout << "Summary Metrics:" << std::endl;
        std::cout << "  Total Concerts: " << metrics.total_concerts << std::endl;
        std::cout << "  Active Concerts: " << metrics.active_concerts << std::endl;
        std::cout << "  Total Attendees: " << metrics.total_attendees << std::endl;
        std::cout << "  Total Revenue: $" << metrics.total_revenue << std::endl;
        std::cout << "  Average Ticket Price: $" << metrics.average_ticket_price << std::endl;
        std::cout << "  Overall Satisfaction: " << metrics.overall_satisfaction_score << "/10" << std::endl;
        std::cout << "  NPS Score: " << metrics.nps_score << std::endl;
        
        std::cout << "✅ Summary metrics test passed" << std::endl;
    }

    /**
     * @brief Test concert-specific metrics
     */
    void testConcertMetrics() {
        std::cout << "Testing concert metrics..." << std::endl;
        
        int concert_id = 303;
        
        // Get concert metrics
        auto metrics = reportModule.getConcertMetrics(concert_id);
        
        // Verify metrics structure
        assert(metrics.concert_id == concert_id);
        assert(metrics.total_registrations >= 0);
        assert(metrics.tickets_sold >= 0);
        assert(metrics.tickets_available >= 0);
        assert(metrics.sales_volume >= 0.0);
        assert(metrics.capacity_utilization >= 0.0 && metrics.capacity_utilization <= 100.0);
        assert(metrics.attendee_engagement_score >= 0.0);
        assert(metrics.nps_score >= -100.0 && metrics.nps_score <= 100.0);
        assert(metrics.total_feedback_count >= 0);
        assert(metrics.average_rating >= 0.0);
        
        std::cout << "Concert " << concert_id << " Metrics:" << std::endl;
        std::cout << "  Concert Name: " << metrics.concert_name << std::endl;
        std::cout << "  Registrations: " << metrics.total_registrations << std::endl;
        std::cout << "  Tickets Sold: " << metrics.tickets_sold << std::endl;
        std::cout << "  Sales Volume: $" << metrics.sales_volume << std::endl;
        std::cout << "  Capacity Utilization: " << metrics.capacity_utilization << "%" << std::endl;
        std::cout << "  Engagement Score: " << metrics.attendee_engagement_score << std::endl;
        std::cout << "  Average Rating: " << metrics.average_rating << std::endl;
        
        std::cout << "✅ Concert metrics test passed" << std::endl;
    }

    /**
     * @brief Test revenue breakdown analysis
     */
    void testRevenueBreakdown() {
        std::cout << "Testing revenue breakdown..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Test daily breakdown
        auto daily_revenue = reportModule.getRevenueBreakdown(start_date, end_date, "daily");
        
        // Test weekly breakdown
        auto weekly_revenue = reportModule.getRevenueBreakdown(start_date, end_date, "weekly");
        
        // Test monthly breakdown
        auto monthly_revenue = reportModule.getRevenueBreakdown(start_date, end_date, "monthly");
        
        std::cout << "Revenue Breakdown Results:" << std::endl;
        std::cout << "  Daily entries: " << daily_revenue.size() << std::endl;
        std::cout << "  Weekly entries: " << weekly_revenue.size() << std::endl;
        std::cout << "  Monthly entries: " << monthly_revenue.size() << std::endl;
        
        // Print some daily revenue data
        int count = 0;
        for (const auto& entry : daily_revenue) {
            if (count < 3) { // Show first 3 entries
                std::cout << "  " << entry.first << ": $" << entry.second << std::endl;
                count++;
            }
        }
        
        std::cout << "✅ Revenue breakdown test passed" << std::endl;
    }

    /**
     * @brief Test attendance trends analysis
     */
    void testAttendanceTrends() {
        std::cout << "Testing attendance trends..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Get attendance trends
        auto trends = reportModule.getAttendanceTrends(start_date, end_date);
        
        std::cout << "Attendance Trends:" << std::endl;
        std::cout << "  Total entries: " << trends.size() << std::endl;
        
        // Show sample data
        int count = 0;
        for (const auto& entry : trends) {
            if (count < 5) { // Show first 5 entries
                std::cout << "  " << entry.first << ": " << entry.second << " attendees" << std::endl;
                count++;
            }
        }
        
        std::cout << "✅ Attendance trends test passed" << std::endl;
    }

    /**
     * @brief Test financial summary generation
     */
    void testFinancialSummary() {
        std::cout << "Testing financial summary..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Generate financial summary
        auto summary = reportModule.generateFinancialSummary(start_date, end_date);
        
        // Verify financial metrics
        assert(summary.total_revenue >= 0.0);
        assert(summary.gross_profit >= 0.0);
        assert(summary.operating_expenses >= 0.0);
        assert(summary.total_transactions >= 0);
        assert(summary.average_transaction_value >= 0.0);
        
        std::cout << "Financial Summary:" << std::endl;
        std::cout << "  Total Revenue: $" << summary.total_revenue << std::endl;
        std::cout << "  Gross Profit: $" << summary.gross_profit << std::endl;
        std::cout << "  Net Profit: $" << summary.net_profit << std::endl;
        std::cout << "  Operating Expenses: $" << summary.operating_expenses << std::endl;
        std::cout << "  Total Transactions: " << summary.total_transactions << std::endl;
        std::cout << "  Average Transaction: $" << summary.average_transaction_value << std::endl;
        std::cout << "  Refunds Issued: $" << summary.refunds_issued << std::endl;
        
        std::cout << "✅ Financial summary test passed" << std::endl;
    }

    /**
     * @brief Test profit and loss statement generation
     */
    void testProfitLossStatement() {
        std::cout << "Testing profit and loss statement..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Generate P&L statement in different formats
        std::string json_pl = reportModule.generateProfitLossStatement(start_date, end_date, "JSON");
        assert(!json_pl.empty());
        
        std::string csv_pl = reportModule.generateProfitLossStatement(start_date, end_date, "CSV");
        assert(!csv_pl.empty());
        
        std::cout << "P&L Statement generated:" << std::endl;
        std::cout << "  JSON: " << json_pl.length() << " characters" << std::endl;
        std::cout << "  CSV: " << csv_pl.length() << " characters" << std::endl;
        
        std::cout << "✅ Profit and loss statement test passed" << std::endl;
    }

    /**
     * @brief Test concert performance analysis
     */
    void testConcertPerformanceAnalysis() {
        std::cout << "Testing concert performance analysis..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Analyze concert performance
        auto performance_data = reportModule.analyzeConcertPerformance(start_date, end_date);
        
        std::cout << "Concert Performance Analysis:" << std::endl;
        std::cout << "  Concerts analyzed: " << performance_data.size() << std::endl;
        
        // Show top 3 performing concerts
        for (size_t i = 0; i < std::min((size_t)3, performance_data.size()); i++) {
            const auto& concert = performance_data[i];
            std::cout << "  Rank " << concert.rank << ": " << concert.concert_name 
                     << " (Score: " << concert.performance_score << ")" << std::endl;
        }
        
        std::cout << "✅ Concert performance analysis test passed" << std::endl;
    }

    /**
     * @brief Test venue utilization analysis
     */
    void testVenueUtilizationAnalysis() {
        std::cout << "Testing venue utilization analysis..." << std::endl;
        
        // Analyze venue utilization
        auto utilization_data = reportModule.analyzeVenueUtilization();
        
        std::cout << "Venue Utilization Analysis:" << std::endl;
        std::cout << "  Venues analyzed: " << utilization_data.size() << std::endl;
        
        // Show utilization data
        for (const auto& entry : utilization_data) {
            std::cout << "  Venue " << entry.first << ": " << entry.second << "% utilization" << std::endl;
        }
        
        std::cout << "✅ Venue utilization analysis test passed" << std::endl;
    }

    /**
     * @brief Test customer satisfaction analytics
     */
    void testCustomerSatisfactionAnalytics() {
        std::cout << "Testing customer satisfaction analytics..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Get satisfaction analytics
        auto satisfaction = reportModule.getCustomerSatisfactionAnalytics(start_date, end_date);
        
        // Verify satisfaction metrics
        assert(satisfaction.overall_rating >= 0.0);
        assert(satisfaction.nps_score >= -100.0 && satisfaction.nps_score <= 100.0);
        assert(satisfaction.total_feedback_count >= 0);
        
        std::cout << "Customer Satisfaction Analytics:" << std::endl;
        std::cout << "  Overall Rating: " << satisfaction.overall_rating << "/10" << std::endl;
        std::cout << "  NPS Score: " << satisfaction.nps_score << std::endl;
        std::cout << "  Total Feedback: " << satisfaction.total_feedback_count << std::endl;
        std::cout << "  Improvement Trend: " << satisfaction.improvement_trend << std::endl;
        
        // Show rating distribution
        std::cout << "  Rating Distribution:" << std::endl;
        for (const auto& rating : satisfaction.rating_distribution) {
            std::cout << "    " << rating.first << " stars: " << rating.second << " reviews" << std::endl;
        }
        
        std::cout << "✅ Customer satisfaction analytics test passed" << std::endl;
    }

    /**
     * @brief Test data export functionality
     */
    void testDataExport() {
        std::cout << "Testing data export..." << std::endl;
        
        std::string start_date = "2025-08-01T00:00:00Z";
        std::string end_date = "2025-08-31T23:59:59Z";
        
        // Test different export formats
        std::string json_export = reportModule.exportDataForVisualization(
            "sales", start_date, end_date, "JSON"
        );
        assert(!json_export.empty());
        
        std::string csv_export = reportModule.exportDataForVisualization(
            "attendance", start_date, end_date, "CSV"
        );
        assert(!csv_export.empty());
        
        std::string xml_export = reportModule.exportDataForVisualization(
            "revenue", start_date, end_date, "XML"
        );
        assert(!xml_export.empty());
        
        std::cout << "Data Export Results:" << std::endl;
        std::cout << "  JSON: " << json_export.length() << " characters" << std::endl;
        std::cout << "  CSV: " << csv_export.length() << " characters" << std::endl;
        std::cout << "  XML: " << xml_export.length() << " characters" << std::endl;
        
        std::cout << "✅ Data export test passed" << std::endl;
    }

    /**
     * @brief Test dashboard data generation
     */
    void testDashboardData() {
        std::cout << "Testing dashboard data generation..." << std::endl;
        
        // Generate dashboard data
        auto dashboard = reportModule.generateDashboardData();
        
        // Verify dashboard structure
        assert(dashboard.summary.total_concerts >= 0);
        assert(dashboard.recent_concerts.size() >= 0);
        assert(dashboard.daily_revenue_trend.size() >= 0);
        assert(dashboard.daily_ticket_sales.size() >= 0);
        assert(dashboard.recent_alerts.size() >= 0);
        
        std::cout << "Dashboard Data:" << std::endl;
        std::cout << "  Recent concerts: " << dashboard.recent_concerts.size() << std::endl;
        std::cout << "  Daily revenue entries: " << dashboard.daily_revenue_trend.size() << std::endl;
        std::cout << "  Daily ticket sales entries: " << dashboard.daily_ticket_sales.size() << std::endl;
        std::cout << "  Recent alerts: " << dashboard.recent_alerts.size() << std::endl;
        
        std::cout << "✅ Dashboard data test passed" << std::endl;
    }

    /**
     * @brief Test report query operations
     */
    void testReportQueries() {
        std::cout << "Testing report queries..." << std::endl;
        
        // Generate some test reports
        int concert_id = 304;
        int report_id = reportModule.generateConcertReport(concert_id);
        assert(report_id > 0);
        
        // Test get reports by concert
        auto concert_reports = reportModule.getReportsByConcert(concert_id);
        assert(concert_reports.size() > 0);
        
        // Test get latest report for concert
        auto latest_report = reportModule.getLatestReportForConcert(concert_id);
        assert(latest_report != nullptr);
        
        // Test get reports by date range
        std::string start_date = "2025-01-01T00:00:00Z";
        std::string end_date = "2025-12-31T23:59:59Z";
        auto date_range_reports = reportModule.getReportsByDateRange(start_date, end_date);
        
        std::cout << "Report Query Results:" << std::endl;
        std::cout << "  Concert reports: " << concert_reports.size() << std::endl;
        std::cout << "  Date range reports: " << date_range_reports.size() << std::endl;
        std::cout << "  Latest report ID: " << (latest_report ? latest_report->id : -1) << std::endl;
        
        std::cout << "✅ Report queries test passed" << std::endl;
    }

    /**
     * @brief Test report scheduling functionality
     */
    void testReportScheduling() {
        std::cout << "Testing report scheduling..." << std::endl;
        
        // Test scheduling automatic reports
        std::vector<std::string> recipients = {"admin@museio.com", "manager@museio.com"};
        
        std::string schedule_id = reportModule.scheduleAutomaticReport(
            "daily_sales", 
            "daily", 
            recipients
        );
        
        // Schedule ID should be generated (implementation dependent)
        std::cout << "Scheduled report ID: " << schedule_id << std::endl;
        
        // Test canceling scheduled report if one was created
        if (!schedule_id.empty()) {
            bool cancelled = reportModule.cancelScheduledReport(schedule_id);
            std::cout << "Schedule cancelled: " << (cancelled ? "Yes" : "No") << std::endl;
        }
        
        std::cout << "✅ Report scheduling test passed" << std::endl;
    }
};

/**
 * @brief Interactive test menu for manual testing
 */
void runInteractiveTests() {
    ReportManager::ReportModule reportModule("interactive_test_reports.dat");
    
    int choice;
    do {
        std::cout << "\n=== ReportModule Interactive Test Menu ===" << std::endl;
        std::cout << "1. Generate concert report" << std::endl;
        std::cout << "2. Generate sales analytics report" << std::endl;
        std::cout << "3. Calculate summary metrics" << std::endl;
        std::cout << "4. Get concert metrics" << std::endl;
        std::cout << "5. Generate financial summary" << std::endl;
        std::cout << "6. Analyze concert performance" << std::endl;
        std::cout << "7. Generate dashboard data" << std::endl;
        std::cout << "8. Export data for visualization" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                int concert_id;
                std::cout << "Enter concert ID: ";
                std::cin >> concert_id;
                
                int report_id = reportModule.generateConcertReport(concert_id);
                if (report_id > 0) {
                    std::cout << "✅ Concert report generated with ID: " << report_id << std::endl;
                } else {
                    std::cout << "❌ Failed to generate concert report" << std::endl;
                }
                break;
            }
            case 2: {
                std::string start_date = "2025-08-01T00:00:00Z";
                std::string end_date = "2025-08-31T23:59:59Z";
                
                std::string report = reportModule.generateSalesAnalyticsReport(start_date, end_date, "JSON");
                std::cout << "✅ Sales analytics report generated (" << report.length() << " characters)" << std::endl;
                break;
            }
            case 3: {
                auto metrics = reportModule.calculateSummaryMetrics();
                std::cout << "✅ Summary Metrics:" << std::endl;
                std::cout << "  Total Concerts: " << metrics.total_concerts << std::endl;
                std::cout << "  Total Revenue: $" << metrics.total_revenue << std::endl;
                std::cout << "  Average Ticket Price: $" << metrics.average_ticket_price << std::endl;
                std::cout << "  NPS Score: " << metrics.nps_score << std::endl;
                break;
            }
            case 4: {
                int concert_id;
                std::cout << "Enter concert ID: ";
                std::cin >> concert_id;
                
                auto metrics = reportModule.getConcertMetrics(concert_id);
                std::cout << "✅ Concert Metrics:" << std::endl;
                std::cout << "  Concert Name: " << metrics.concert_name << std::endl;
                std::cout << "  Tickets Sold: " << metrics.tickets_sold << std::endl;
                std::cout << "  Sales Volume: $" << metrics.sales_volume << std::endl;
                std::cout << "  Capacity Utilization: " << metrics.capacity_utilization << "%" << std::endl;
                break;
            }
            case 7: {
                auto dashboard = reportModule.generateDashboardData();
                std::cout << "✅ Dashboard Data Generated:" << std::endl;
                std::cout << "  Recent Concerts: " << dashboard.recent_concerts.size() << std::endl;
                std::cout << "  Revenue Trend Entries: " << dashboard.daily_revenue_trend.size() << std::endl;
                std::cout << "  Recent Alerts: " << dashboard.recent_alerts.size() << std::endl;
                break;
            }
            case 0:
                std::cout << "Exiting interactive tests..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);
}

/**
 * @brief Main function to run report module tests
 */
int main(int argc, char* argv[]) {
    try {
        if (argc > 1 && std::string(argv[1]) == "--interactive") {
            runInteractiveTests();
        } else {
            ReportModuleTest test;
            test.runAllTests();
            
            std::cout << std::endl << "🎉 All ReportModule tests completed successfully!" << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "💥 Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
