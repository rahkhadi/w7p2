/**********************************************************************************************************************************
				<assessment name: Workshop - #7 (Part-2)>
Full Name  : Rahimullah Khadim Hussain
Student ID#: 119515229
Email      : rkhadim-hussain@myseneca.ca
Section    : NEE
Date of Completion: 11-11-23

Authenticity Declaration:
 I have done all the coding by myself and only copied the code that my professor provided to complete my workshops and assignments.
***********************************************************************************************************************************/

#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include "CovidCollection.h"


namespace sdds {
	CovidCollection::CovidCollection(const std::string filename) {
		if (!filename.empty()) {
			readFile(filename);
		}
	}

	void CovidCollection::readFile(std::string filename) {
		std::fstream file(filename);
		if (file) {
			std::string line{};
			while (std::getline(file, line)) {
				line = trim(line);
				std::istringstream iss(line);
				Covid temp_covid;
				temp_covid.m_country = trim(line.substr(0, 25));
				temp_covid.m_city = trim(line.substr(25, 25));
				temp_covid.m_variant = trim(line.substr(50, 25));
				temp_covid.m_year = std::stoi(line.substr(75, 5));
				temp_covid.m_cases = std::stoul(line.substr(80, 5));
				temp_covid.m_death = std::stoul(line.substr(85, 5));

				covidCollection.push_back(temp_covid);
			}
		}
		else {
			throw std::invalid_argument("Could not open the specified file for reading.");
		}
	}

	void CovidCollection::display(std::ostream& out, const std::string& country) const {
		int totalCases{}, totalDeath{}, countryCases{}, countryDeath{};
		auto worldCaseDeath = [&totalCases, &totalDeath](int cases, int deaths) {
			totalCases += cases;
			totalDeath += deaths;
		};

		auto countryCaseDeath = [&countryCases, &countryDeath](int cases, int deaths) {
			countryCases += cases;
			countryDeath += deaths;
		};

		if (country != "ALL") {
			out << "Displaying information of country = " << country << std::endl;
		}

		std::for_each(covidCollection.begin(), covidCollection.end(), [&worldCaseDeath, &countryCaseDeath, &country, &out](Covid covid) {
			worldCaseDeath(covid.m_cases, covid.m_death);
			if (country == "ALL" || covid.m_country == country) {
				countryCaseDeath(covid.m_cases, covid.m_death);
				out << covid << std::endl;
			}
			});

		if (country == "ALL") {
			out << "| " << std::right << std::setw(79) << "Total cases around the world: " << totalCases << " |" << std::endl;
			out << "| " << std::right << std::setw(79) << "Total deaths around the world: " << totalDeath << " |" << std::endl;
		}
		else {
			std::string countryCase = "Total cases in " + country + ": " + std::to_string(countryCases);
			std::string countryDeaths = "Total deaths in " + country + ": " + std::to_string(countryDeath);
			std::string percentageInfo = country + " has " + std::to_string(static_cast<double>(countryCases) / totalCases * 100) + "% of global cases and " + std::to_string(static_cast<double>(countryDeath) / totalDeath * 100) + "% of global deaths";

			out << std::setfill('-') << std::setw(88) << "" <<std::endl << std::setfill(' ');
			out << "| " << std::right << std::setw(84) << countryCase << " |" << std::endl;
			out << "| " << std::right << std::setw(84) << countryDeaths << " |" << std::endl;
			out << "| " << std::right << std::setw(84) << percentageInfo << " |" << std::endl;
		}
	}
	
	void CovidCollection::sort(const std::string& field) {
		std::sort(covidCollection.begin(), covidCollection.end(), [field](Covid& a, Covid& b) {
			if (field == "country") {
				return a.m_country < b.m_country || (a.m_country == b.m_country && a.m_death < b.m_death);
			}
			else if (field == "city") {
				return a.m_city < b.m_city || (a.m_city == b.m_city && a.m_death < b.m_death);
			}
			else if (field == "variant") {
				return a.m_variant < b.m_variant || (a.m_variant == b.m_variant && a.m_death < b.m_death);
			}
			else if (field == "year") {
				return a.m_year < b.m_year || (a.m_year == b.m_year && a.m_death < b.m_death);
			}
			else if (field == "cases") {
				return a.m_cases < b.m_cases || (a.m_cases == b.m_cases && a.m_death < b.m_death);
			}
			else if (field == "deaths") {
				return a.m_death < b.m_death || (a.m_death == b.m_death && a.m_country < b.m_country);
			}
			else {
				return a.m_country < b.m_country || (a.m_country == b.m_country && a.m_death < b.m_death);
			}
			});
	}

	bool CovidCollection::inCollection(const std::string& variant, const std::string& country, unsigned int deaths) const {
		bool result = std::find_if(covidCollection.begin(), covidCollection.end(), [variant, country, deaths](const Covid& a) {
			if (a.m_country == country && a.m_variant == variant && a.m_death > deaths) {
				return true;
			}
			return false;
			}) != covidCollection.end();
		return result;
	}

	std::list<Covid> CovidCollection::getListForDeaths(unsigned int deaths) const {
		std::list<Covid> resultList{};
		std::copy_if(covidCollection.begin(), covidCollection.end(), std::back_inserter(resultList),
			[deaths](const Covid& covid) {
				return covid.m_death >= deaths;
			});
		return resultList;
	}

	void CovidCollection::updateStatus() {
		std::for_each(covidCollection.begin(), covidCollection.end(), [](Covid& covid) {
			if (covid.m_death > 300) {
				covid.severity = "EPIDEMIC";
			}
			else if (covid.m_death < 50) {
				covid.severity = "EARLY";
			}
			else {
				covid.severity = "MILD";
			}
			});
	}

	std::ostream& operator<<(std::ostream& out, const Covid& theCovid) {
		out << "| " << std::left << std::setw(21) << theCovid.m_country << " | "
			<< std::left << std::setw(15) << theCovid.m_city << " | "
			<< std::left << std::setw(20) << theCovid.m_variant << " | ";
		if (theCovid.m_year >= 0) {
			out << std::right << std::setw(6) << theCovid.m_year;
		}
		else {
			out << std::setw(6) << " ";
		}
		out << " | "
			<< std::right << std::setw(4) << theCovid.m_cases << " | "
			<< std::right << std::setw(3) << theCovid.m_death << " | | "
			<< std::right << std::setw(8) << theCovid.severity << " |";
		return out;
	}

	std::string trim(const std::string& str) {
		size_t start = str.find_first_not_of(" ");
		size_t end = str.find_last_not_of(" ");

		if (start == std::string::npos || end == std::string::npos)
			return "";

		return str.substr(start, end - start + 1);
	}
}
