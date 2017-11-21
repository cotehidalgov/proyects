class Expense < ApplicationRecord
  	belongs_to :building
    belongs_to :apartment
end
