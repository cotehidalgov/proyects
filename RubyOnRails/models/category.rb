class Category < ApplicationRecord
	has_many :proyects

	validates :name, presence: true, allow_blank: false
end
