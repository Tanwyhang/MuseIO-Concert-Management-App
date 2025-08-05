#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include "models.hpp"
#include "baseModule.hpp"

namespace ReportManager {

    /**
     * @brief Report Module for generating analytics and reports
     * 
     * This module handles generation of various reports including sales reports,
     * analytics, performance metrics, and data visualization for the MuseIO Concert Management System.
     */
    class ReportModule : public BaseModule<Model::ConcertReport, int> {
    public:
        /**
         * @brief Constructor
         * @param filePath Path to the reports data file
         */
        ReportModule(const std::string& filePath = "data/reports.dat");

        /**
         * @brief Destructor
         */
        ~ReportModule() = default;

        // Core Report Operations

        /**
         * @brief Generate a comprehensive concert report
         * @param concert_id Concert ID to generate report for
         * @return Report ID if successful, -1 if failed
         */
        int generateConcertReport(int concert_id);

        /**
         * @brief Update an existing concert report
         * @param report_id Report ID to update
         * @return true if successful, false otherwise
         */
        bool updateConcertReport(int report_id);

        /**
         * @brief Generate sales and analytics report for a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param format Output format ("JSON", "CSV", "PDF", "HTML")
         * @return Generated report content as string
         */
        std::string generateSalesAnalyticsReport(const std::string& start_date,
                                               const std::string& end_date,
                                               const std::string& format = "JSON");

        /**
         * @brief Generate payroll report for crew and staff
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param format Output format ("JSON", "CSV", "PDF")
         * @return Payroll report content as string
         */
        std::string generatePayrollReport(const std::string& start_date,
                                        const std::string& end_date,
                                        const std::string& format = "JSON");

        // Analytics and Metrics

        /**
         * @brief Calculate overall summary metrics for all concerts
         * @return Summary metrics struct
         */
        struct SummaryMetrics {
            int total_concerts;
            int active_concerts;
            int completed_concerts;
            int cancelled_concerts;
            int total_attendees;
            int total_tickets_sold;
            double total_revenue;
            double average_ticket_price;
            double overall_satisfaction_score;
            double nps_score;
            std::string most_popular_concert;
            std::string top_performing_venue;
        };
        SummaryMetrics calculateSummaryMetrics();

        /**
         * @brief Get metrics for a specific concert
         * @param concert_id Concert ID
         * @return Concert-specific metrics struct
         */
        struct ConcertMetrics {
            int concert_id;
            std::string concert_name;
            int total_registrations;
            int tickets_sold;
            int tickets_available;
            double sales_volume;
            double capacity_utilization;
            double attendee_engagement_score;
            double nps_score;
            int total_feedback_count;
            double average_rating;
            std::string top_payment_method;
            Model::DateTime last_updated;
        };
        ConcertMetrics getConcertMetrics(int concert_id);

        /**
         * @brief Get revenue breakdown by time period
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param period_type Period type ("daily", "weekly", "monthly")
         * @return Map of period -> revenue amount
         */
        std::map<std::string, double> getRevenueBreakdown(const std::string& start_date,
                                                        const std::string& end_date,
                                                        const std::string& period_type = "daily");

        /**
         * @brief Get attendance trends over time
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Map of date -> attendance count
         */
        std::map<std::string, int> getAttendanceTrends(const std::string& start_date,
                                                     const std::string& end_date);

        // Financial Reports

        /**
         * @brief Generate financial summary report
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Financial summary struct
         */
        struct FinancialSummary {
            double total_revenue;
            double gross_profit;
            double net_profit;
            double operating_expenses;
            double venue_costs;
            double performer_fees;
            double marketing_costs;
            double staff_costs;
            double payment_processing_fees;
            double refunds_issued;
            int total_transactions;
            double average_transaction_value;
            std::map<std::string, double> revenue_by_currency;
            std::map<std::string, double> revenue_by_payment_method;
        };
        FinancialSummary generateFinancialSummary(const std::string& start_date,
                                                const std::string& end_date);

        /**
         * @brief Generate profit and loss statement
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param format Output format ("JSON", "CSV", "PDF")
         * @return P&L statement content
         */
        std::string generateProfitLossStatement(const std::string& start_date,
                                              const std::string& end_date,
                                              const std::string& format = "JSON");

        // Performance Analytics

        /**
         * @brief Analyze concert performance and ranking
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Vector of concerts ranked by performance
         */
        struct ConcertPerformance {
            int concert_id;
            std::string concert_name;
            double performance_score;
            int tickets_sold;
            double revenue;
            double satisfaction_score;
            int rank;
        };
        std::vector<ConcertPerformance> analyzeConcertPerformance(const std::string& start_date,
                                                                const std::string& end_date);

        /**
         * @brief Analyze venue utilization rates
         * @return Map of venue_id -> utilization percentage
         */
        std::map<int, double> analyzeVenueUtilization();

        /**
         * @brief Get customer satisfaction analytics
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Customer satisfaction metrics
         */
        struct SatisfactionMetrics {
            double overall_rating;
            double nps_score;
            int total_feedback_count;
            std::map<int, int> rating_distribution; // rating -> count
            std::vector<std::string> top_positive_comments;
            std::vector<std::string> top_concerns;
            double improvement_trend;
        };
        SatisfactionMetrics getCustomerSatisfactionAnalytics(const std::string& start_date,
                                                           const std::string& end_date);

        // Export and Visualization

        /**
         * @brief Export data for external visualization tools
         * @param report_type Type of report to export
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @param format Export format ("JSON", "CSV", "XML")
         * @return Exported data as string
         */
        std::string exportDataForVisualization(const std::string& report_type,
                                             const std::string& start_date,
                                             const std::string& end_date,
                                             const std::string& format = "JSON");

        /**
         * @brief Generate dashboard data for real-time monitoring
         * @return Dashboard data struct
         */
        struct DashboardData {
            SummaryMetrics summary;
            std::vector<ConcertMetrics> recent_concerts;
            std::map<std::string, double> daily_revenue_trend;
            std::map<std::string, int> daily_ticket_sales;
            std::vector<std::string> recent_alerts;
            Model::DateTime last_updated;
        };
        DashboardData generateDashboardData();

        // Query Operations

        /**
         * @brief Get report by ID
         * @param report_id Report ID to retrieve
         * @return Shared pointer to report, nullptr if not found
         */
        std::shared_ptr<Model::ConcertReport> getReportById(int report_id);

        /**
         * @brief Get all reports for a specific concert
         * @param concert_id Concert ID
         * @return Vector of reports for the concert
         */
        std::vector<std::shared_ptr<Model::ConcertReport>> getReportsByConcert(int concert_id);

        /**
         * @brief Get reports within a date range
         * @param start_date Start date (ISO 8601 format)
         * @param end_date End date (ISO 8601 format)
         * @return Vector of reports within the date range
         */
        std::vector<std::shared_ptr<Model::ConcertReport>> getReportsByDateRange(
            const std::string& start_date, const std::string& end_date);

        /**
         * @brief Get the latest report for a concert
         * @param concert_id Concert ID
         * @return Shared pointer to latest report, nullptr if not found
         */
        std::shared_ptr<Model::ConcertReport> getLatestReportForConcert(int concert_id);

        // Report Scheduling and Automation

        /**
         * @brief Schedule automatic report generation
         * @param report_type Type of report to schedule
         * @param frequency Frequency ("daily", "weekly", "monthly")
         * @param recipients Email recipients for the report
         * @return Schedule ID if successful, empty string if failed
         */
        std::string scheduleAutomaticReport(const std::string& report_type,
                                          const std::string& frequency,
                                          const std::vector<std::string>& recipients);

        /**
         * @brief Cancel scheduled report
         * @param schedule_id Schedule ID to cancel
         * @return true if successful, false otherwise
         */
        bool cancelScheduledReport(const std::string& schedule_id);

    protected:
        // BaseModule implementation
        int getEntityId(const std::shared_ptr<Model::ConcertReport>& entity) const override;
        void loadEntities() override;
        bool saveEntities() override;

    private:
        /**
         * @brief Calculate engagement score based on various metrics
         * @param concert_id Concert ID
         * @return Engagement score (0.0 to 10.0)
         */
        double calculateEngagementScore(int concert_id);

        /**
         * @brief Calculate Net Promoter Score from feedback
         * @param feedback_scores Vector of feedback ratings
         * @return NPS score (-100 to +100)
         */
        double calculateNPS(const std::vector<int>& feedback_scores);

        /**
         * @brief Format report output based on requested format
         * @param data Report data
         * @param format Output format
         * @return Formatted report string
         */
        std::string formatReportOutput(const std::string& data, const std::string& format);

        /**
         * @brief Validate date range parameters
         * @param start_date Start date string
         * @param end_date End date string
         * @return true if valid, false otherwise
         */
        bool validateDateRange(const std::string& start_date, const std::string& end_date);

        /**
         * @brief Generate HTML report template
         * @param title Report title
         * @param content Report content
         * @return HTML formatted report
         */
        std::string generateHTMLReport(const std::string& title, const std::string& content);

        /**
         * @brief Log report generation for audit trail
         * @param report_type Type of report generated
         * @param parameters Report parameters
         */
        void logReportGeneration(const std::string& report_type, 
                               const std::string& parameters);
    };

}
